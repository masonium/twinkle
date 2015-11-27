#include "multilayered.h"
#include <cmath>

GTR::GTR(scalar roughness, scalar gamma) : _r2(roughness * roughness), _gamma(gamma)
{
  assert(gamma != 1);
}

Vec3 GTR::sample_micronormal(Sampler& sampler, scalar& p) const
{
  auto sample = sampler.sample_2d();
  scalar theta = 2 * PI * sample[0];

  scalar cos_phi = sqrt((1 - pow((pow(_r2, 1.0 - _gamma) * (1.0 - sample[1]) + sample[1]),
                                 1/(1 - _gamma))) / (1 - _r2));

  scalar phi = acos(cos_phi);
  auto h = Vec3::from_euler(theta, phi);

  p = density(h) * h.z;

  return h;
}

scalar GTR::density(const Vec3& h) const
{
  scalar n1 = (_gamma - 1.0) * (_r2 - 1.0);
  scalar d1 = PI * (1.0 - pow(_r2, 1.0 - _gamma));
  scalar d2 = pow(1.0 + (_r2 - 1.0) * h.z * h.z, _gamma);

  return n1 / (d1 * d2);
 }

scalar GTR::g1(const Vec3& v, const Vec3& h) const
{
  if (h.z > 0 && v.dot(h) > 0)
  {
    scalar t2 = 1 / square(v.z) - 1;
    return 2.0 / (1.0 + sqrt(1.0 + _r2 * t2));
  }
  return 0.0;
}

scalar GTR::pdf(const Vec3& incoming, const Vec3& outgoing) const
{
  Vec3 h = (incoming + outgoing).normal();
  if (h.z < 0)
    return 0;

  scalar p = density(h) * h.z / (4.0 * h.dot(outgoing));
  if (p < 0.1)
    return 0;

  return p;
}

scalar GTR::reflectance(scalar schlick_f0, const Vec3& incoming, const Vec3& outgoing, scalar& G) const
{
  auto h = (incoming + outgoing).normal();
  scalar r = h.dot(incoming);

  scalar F = schlick_f0 + (1 - schlick_f0) * pow(1 - r, 5);
  scalar D = density(h);
  G = g1(incoming, h) * g1(outgoing, h);

  return F * D * G / (4.0 * incoming.z * outgoing.z);
}

TransitionSample GTR::sample_direction(const Vec3& incoming, Sampler& sampler) const
{
  scalar p;
  auto h = sample_micronormal(sampler, p);

  TransitionSample ts;
  ts.reflection_dir = (2 * (incoming.dot(h))) * h - incoming;
  ts.p = (ts.reflection_dir.z > 0) ? pdf(incoming, ts.reflection_dir) : 0;

  return ts;
}

////////////////////////////////////////////////////////////////////////////////

spectrum MFLayer::reflectance(const Vec3& incoming, const Vec3& outgoing, scalar& G) const
{
  return spectrum{_tld->reflectance(_n_sf0, incoming, outgoing, G)};
}

TransitionSample MFLayer::sample_bsdf(const Vec3& incoming, Sampler& sampler) const
{
  return _tld->sample_direction(incoming, sampler);
}

scalar MFLayer::pdf(const Vec3& incoming, const Vec3& outgoing) const
{
  return _tld->pdf(incoming, outgoing);
}

////////////////////////////////////////////////////////////////////////////////

LayeredMFMaterial::LayeredMFMaterial(const vector<shared_ptr<MFLayer>>& layers, shared_ptr<Material> base)
  : _layers(layers), _base(base)
{
}

spectrum LayeredMFMaterial::reflectance(const IntersectionView& isect, const Vec3& incoming, const Vec3& outgoing) const
{
  return reflectance(isect, incoming, outgoing, _layers.size() - 1);
}

spectrum LayeredMFMaterial::reflectance(const IntersectionView& isect, const Vec3& incoming, const Vec3& outgoing,
                                        int layer_index) const
{
  if (incoming.z < 0 || outgoing.z < 0)
    return spectrum::zero;

  if (layer_index < 0)
    return _base->reflectance(isect, incoming, outgoing);

  scalar G;
  auto& layer = _layers[layer_index];
  auto outer_refl = layer->reflectance(incoming, outgoing, G);
  scalar n1 = layer->_n_outside, n2 = layer->_n_inside;
  scalar T = fresnel_transmittance_schlick(incoming, Vec3::z_axis, n1, n2);
  assert(T >= 0.0);
  assert(T <= 1.0);
  scalar T12 = square(n2 / n1) * T;

  auto refr_incoming = refraction_direction(incoming, Vec3::z_axis, n1, n2);
  auto refr_outgoing = refraction_direction(outgoing, Vec3::z_axis, n1, n2);

  auto inner_refl = reflectance(isect, -refr_incoming, -refr_outgoing, layer_index - 1);

  scalar TIR = (1 - G) + G * square(n1 / n2) * T;

  scalar path_length = 1.0 / refr_outgoing.z + 1.0 / refr_incoming.z;
  scalar absorption = exp(-layer->_absorption * path_length);

  return outer_refl + T12 * layer->_tint * inner_refl * absorption * TIR;
  //return spectrum{T12};
}

scalar LayeredMFMaterial::pdf(const Vec3& incoming, const Vec3& outgoing) const
{
  return 0;
}
/*
Vec3 LayeredMFMaterial::sample_bsdf(const IntersectionView& view, const Vec3& incoming, Sampler& sampler,
                                    scalar& p, spectrum& reflectance) const
{
  // pick a layer to sample from, uniformly
  uint sample_index = sampler.sample_1d() * (_layers.size() + 1);

  TransitionSample ts;
  spectrum r;

  const scalar total_layers = _layers.size() + 1;

  // sample from that layer
  scalar p_layer = 0;
  if (sample_index < _layers.size())
  {
    ts = _layers[sample_index]->sample_bsdf(incoming, sampler);
    p_layer = ts.p;
  }
  else
  {
    ts.reflection_dir = _base->sample_bsdf(view, incoming, sampler, p_layer, r);
  }

  // compute the total pdf
  for (auto i = 0u; i < _layers.size(); ++i)
  {
    if (i != sample_index)
      p_layer += _layers[i]->pdf(incoming, ts.reflection_dir);
  }
  if (sample_index != _layers.size())
    p_layer += _base->pdf(incoming, ts.reflection_dir);

  reflectance = this->reflectance(view, incoming, ts.reflection_dir);
  p = p_layer / total_layers;

  return ts.reflection_dir;
}
*/

Vec3 LayeredMFMaterial::sample_bsdf(const IntersectionView& view, const Vec3& incoming, Sampler& sampler,
                                    scalar& p, spectrum& reflectance) const
{
  // sample from base layer
  spectrum r;
  Vec3 dir = _base->sample_bsdf(view, incoming, sampler, p, r);

  reflectance = this->reflectance(view, incoming, dir);

  return dir;
}


////////////////////////////////////////////////////////////////////////////////

MaterialTLDAdapter::MaterialTLDAdapter(scalar n_outside, scalar n_inside,
                                       shared_ptr<TransitionLayerDistribution> tld) : _tld(tld)
{
  _sf0 = schlick_r0_term(n_outside, n_inside);
}

scalar MaterialTLDAdapter::pdf(const Vec3& incoming, const Vec3& outgoing) const
{
  return _tld->pdf(incoming, outgoing);
}

spectrum MaterialTLDAdapter::reflectance(const IntersectionView& UNUSED(isect), const Vec3& incoming,
                                         const Vec3& outgoing) const
{
  scalar G;
  return spectrum{_tld->reflectance(_sf0, incoming, outgoing, G)};
}

Vec3 MaterialTLDAdapter::sample_bsdf(const IntersectionView& isect, const Vec3& incoming, Sampler& sampler,
                                     scalar& p, spectrum& reflectance) const
{
  auto ts = _tld->sample_direction(incoming, sampler);
  p = ts.p;
  if (ts.p <= 0)
  {
    reflectance = spectrum::zero;
  }
  else
  {
    reflectance = spectrum{this->reflectance(isect, incoming, ts.reflection_dir)};
  }
  return ts.reflection_dir;
}
