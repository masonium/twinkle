#pragma once

#include "material.h"
#include "sampler.h"
#include "bsdf.h"
#include <vector>

using std::vector;

struct TransitionSample
{
  Vec3 reflection_dir;
  scalar p;
};

class TransitionLayerDistribution
{
public:
  virtual TransitionSample sample_direction(const Vec3& incoming, Sampler& sampler) const = 0;

  virtual scalar reflectance(scalar schlick_f0, const Vec3& incoming, const Vec3& outgoing,
                             scalar& shadowing) const = 0;

  virtual scalar pdf(const Vec3& incoming, const Vec3& outgoing) const = 0;
};

using TLD = TransitionLayerDistribution;


/*
 * The Generalized Trowbridge-Reitz distribution is described in detail in the
 * Disney BDRF paper:
 *
 * http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf
 *
 * It is equivalent to Trowbridge-Reitz at gamma = 1 and GGX at gamma = 2.
 *
 * The self-shadowing function using the GXX g1-function as described in
 * "Microfacet Models for Refraction through Rough Surfaces"
 *
 * http://www.graphics.cornell.edu/~bjw/microfacetbsdf.pdf
 */
class GTR : public TransitionLayerDistribution
{
public:
  GTR(scalar roughness, scalar gamma = 2.0);

  TransitionSample sample_direction(const Vec3& incoming, Sampler& sampler) const override;

  scalar reflectance(scalar schlick_f0, const Vec3& incoming,
                     const Vec3& outgoing, scalar& shadowing) const override;

  scalar pdf(const Vec3& incoming, const Vec3& outgoing) const override;
  Vec3 sample_micronormal(Sampler& sampler, scalar& p) const;

private:
  scalar density(const Vec3& h) const;

  scalar g1(const Vec3& v, const Vec3& h) const;

  const scalar _r2;
  const scalar _gamma;
};

/*
 * Varnish/top layer for the multifacet model.
 */
class MFLayer
{
public:
  MFLayer(scalar a, spectrum tint, shared_ptr<TLD> dist) :
    _tint(tint), _absorption(a), _n_outside(refraction_index::AIR),
    _n_inside(refraction_index::CROWN_GLASS),
    _n_sf0(schlick_r0_term(_n_outside, _n_inside)), _tld(dist)
  {
  }
  spectrum reflectance(const Vec3& incoming, const Vec3& outgoing, scalar& G) const;

  TransitionSample sample_bsdf(const Vec3& incoming, Sampler& sampler) const;

  scalar pdf(const Vec3& incoming, const Vec3& outgoing) const;

  spectrum _tint;
  scalar _absorption;
  scalar _n_outside;
  scalar _n_inside;
  scalar _n_sf0;
  shared_ptr<TransitionLayerDistribution> _tld;
};


/*
 * A multilayered material, based on the model in:
 *
 * "Arbitrarily Layered Micro-Facet Surfaces" by Andrea Weidlich and Alexander
 * Wilkie
 *
 * The material consists of one or more microfacet transition layers, stacked on
 * top of a base layer.
 */

class LayeredMFMaterial : public Material
{
public:
  LayeredMFMaterial(const vector<shared_ptr<MFLayer>>& layers, Material* base);

  spectrum reflectance(const IntersectionView&, const Vec3& incoming, const Vec3& outgoing) const override;

  Vec3 sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                   scalar& p, spectrum& reflectance) const override;

  scalar pdf(const Vec3& incoming, const Vec3& outgoing) const override;

  std::string to_string() const override { return "LayeredMicrofacetMaterial"; }

private:
  spectrum reflectance(const IntersectionView&, const Vec3& incoming, const Vec3& outgoing,
                       int layer_index) const;

  vector<shared_ptr<MFLayer>> _layers;
  Material* _base;
  vector<scalar> cum_sample_probs;
};

/*
 * Utility class for adapting a microfacet distribution directly as a specular
 * BRDF.
 */
class MaterialTLDAdapter : public Material
{
public:
  MaterialTLDAdapter(scalar n_outside, scalar n_inside, shared_ptr<TransitionLayerDistribution> tld);

  spectrum reflectance(const IntersectionView&, const Vec3& incoming, const Vec3& outgoing) const override;

  scalar pdf(const Vec3& incoming, const Vec3& outgoing) const override;

  Vec3 sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                   scalar& p, spectrum& reflectance) const override;

  std::string to_string() const override { return "Material Adapter"; }

private:
  scalar _sf0;
  shared_ptr<TransitionLayerDistribution> _tld;
};
