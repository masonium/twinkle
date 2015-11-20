#include "multilayered.h"
#include <cmath>

GTR::GTR(scalar roughness, scalar gamma) : _r2(roughness * roughness), _gamma(gamma)
{
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
    return 2.0 / (1.0 + sqrt(1.0 + _r2 * (1 - 1 / v.z * v.z)));
  return 0.0;
}

scalar GTR::pdf(const Vec3& incoming, const Vec3& outgoing) const
{
  Vec3 h = (incoming + outgoing).normal();
  if (h.z < 0)
    return 0;

  return density(h) * h.z / (h.dot(outgoing));
}

scalar GTR::reflectance(scalar schlick_f0, const Vec3& incoming, const Vec3& outgoing, scalar& G) const
{
  auto h = (incoming + outgoing).normal();
  scalar r = h.dot(incoming);

  scalar F = schlick_f0 + (1 - schlick_f0) * pow(1 - r*r, 5);
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
  ts.p = (ts.reflection_dir.z > 0) ? p : 0;

  return ts;
}

////////////////////////////////////////////////////////////////////////////////

spectrum LayeredMFMaterial::MFLayer::reflectance(const Vec3& incoming, const Vec3& outgoing, scalar& G)
{
  return _tint * _tld->reflectance(_n_sf0, incoming, outgoing, G);
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
  if (layer_index < 0)
    return _base->reflectance(isect, incoming, outgoing);

  scalar G;
  auto outer_refl = _layers[layer_index]->reflectance(incoming, outgoing, G);
//  scalar T1 = square(_n_outside / _n_inside) * (1 - fresnel_transmittance
  return spectrum{0.0};
}

Vec3 LayeredMFMaterial::sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                                    scalar& p, spectrum& reflectance) const
{
  p = 0;
  reflectance = spectrum::zero;
  return Vec3::zero;
}


////////////////////////////////////////////////////////////////////////////////

MaterialTLDAdapter::MaterialTLDAdapter(scalar n_outside, scalar n_inside,
                                       shared_ptr<TransitionLayerDistribution> tld) : _tld(tld)
{
  _sf0 = schlick_r0_term(n_outside, n_inside);
}

spectrum MaterialTLDAdapter::reflectance(const IntersectionView& UNUSED(isect), const Vec3& incoming,
                                         const Vec3& outgoing) const
{
  scalar G;
  return spectrum{_tld->reflectance(_sf0, incoming, outgoing, G)};
}

Vec3 MaterialTLDAdapter::sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                                     scalar& p, spectrum& reflectance) const
{
  auto ts = _tld->sample_direction(incoming, sampler);
  p = ts.p;
  if (ts.p < 0)
  {
    reflectance = spectrum::zero;
  }
  else
  {
    scalar G;
    reflectance = spectrum{_tld->reflectance(_sf0, incoming, ts.reflection_dir, G)};
  }
  return ts.reflection_dir;
}
