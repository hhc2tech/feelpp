//! -*- mode: c++; coding: utf-8; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; show-trailing-whitespace: t  -*-
//!
//! This file is part of the Feel++ library
//!
//! This library is free software; you can redistribute it and/or
//! modify it under the terms of the GNU Lesser General Public
//! License as published by the Free Software Foundation; either
//! version 2.1 of the License, or (at your option) any later version.
//!
//! This library is distributed in the hope that it will be useful,
//! but WITHOUT ANY WARRANTY; without even the implied warranty of
//! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//! Lesser General Public License for more details.
//!
//! You should have received a copy of the GNU Lesser General Public
//! License along with this library; if not, write to the Free Software
//! Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//!
//! @file
//! @author <you>
//! @date 15 Jun 2017
//! @copyright 2017 Feel++ Consortium
//!
//!

#ifndef FEELPP_ALPHATHERMOELECTRIC_HPP
#define FEELPP_ALPHATHERMOELECTRIC_HPP

#include <feel/feelcrb/modelcrbbase.hpp>
#include <feel/options.hpp>
#include <feel/feelmodels/modelproperties.hpp>
#include <feel/feelcrb/deim.hpp>

namespace Feel
{

FEELPP_EXPORT po::options_description
makeOptions()
{
    po::options_description options( "Thermoelectric" );
    options.add_options()
        ( "thermoelectric.filename", Feel::po::value<std::string>()->default_value("thermoelectric.json"),
          "json file containing application parameters and boundary conditions")
        ( "thermoelectric.gamma", po::value<double>()->default_value( 1e5 ), "penalisation term" )
        ( "thermoelectric.trainset-deim-size", po::value<int>()->default_value(40), "size of the deim trainset" )
        ( "thermoelectric.trainset-mdeim-size", po::value<int>()->default_value(40), "size of the mdeim trainset" )
        ;
    options.add(backend_options("feV") ).add(backend_options("feT") );
    options.add(deimOptions("vec")).add(deimOptions("mat"));

    return options;
}

FEELPP_EXPORT AboutData
makeAbout( std::string const& str = "thermoelectriccrbmodel" )
{
    AboutData about( str.c_str() );
    return about;
}

class ParameterDefinition
{
public:
    using parameterspace_type = ParameterSpaceX;
};

class FunctionSpaceDefinition
{
public:
    using value_type = double;
    static const uint16_type Order = 1;
    using convex_type =  Simplex<3>;
    using mesh_type = Mesh<convex_type>;
    using fct_base_type = Lagrange<Order,Scalar>;
    using basis_type = bases<fct_base_type, fct_base_type>;
    using space_type = FunctionSpace<mesh_type, basis_type, value_type>;
    using element_type = typename space_type::element_type;

    using V_basis_type = bases<fct_base_type>;
    using V_space_type = FunctionSpace<mesh_type, V_basis_type, value_type>;

    using J_basis_type = bases<Lagrange<Order+2, Scalar,Discontinuous> >;
    using J_space_type = FunctionSpace<mesh_type, J_basis_type, Discontinuous, value_type>;

    using mdeim_basis_type = bases<Lagrange<1,Tensor2> >;
    using mdeim_space_type = FunctionSpace<mesh_type, mdeim_basis_type>;
    using mdeim_space_ptrtype = boost::shared_ptr<mdeim_space_type>;
};

template <typename ParameterDefinition, typename FunctionSpaceDefinition >
class EimDefinition
{
public :
    typedef typename ParameterDefinition::parameterspace_type parameterspace_type;
    typedef typename FunctionSpaceDefinition::space_type space_type;
    typedef typename FunctionSpaceDefinition::J_space_type J_space_type;
    using V_space_type = typename FunctionSpaceDefinition::V_space_type;

    /* EIM */
    // Scalar continuous //
    typedef EIMFunctionBase<V_space_type, space_type , parameterspace_type> fun_type;
    // Scalar Discontinuous //
    typedef EIMFunctionBase<J_space_type, space_type , parameterspace_type> fund_type;

};

class FEELPP_EXPORT AlphaThermoelectric : public ModelCrbBase<ParameterDefinition,
                                                              FunctionSpaceDefinition,
                                                              NonLinear,
                                                              EimDefinition<ParameterDefinition,
                                                                            FunctionSpaceDefinition> >
{
public:
    using super_type = ModelCrbBase<ParameterDefinition,
                                    FunctionSpaceDefinition,
                                    NonLinear,
                                    EimDefinition<ParameterDefinition,
                                                  FunctionSpaceDefinition> >;
    using self_type = AlphaThermoelectric;
    using parameterspace_type = ParameterDefinition::parameterspace_type;
    using function_space_type = FunctionSpaceDefinition;
    using eim_definition_type = EimDefinition<ParameterDefinition, FunctionSpaceDefinition>;

    using value_type = double;

    using mesh_type = super_type::mesh_type;
    using mesh_ptrtype = super_type::mesh_ptrtype;

    using space_type = super_type::space_type;
    using element_type = super_type::element_type;
    using element_ptrtype = super_type::element_ptrtype;

    using J_space_type = FunctionSpaceDefinition::J_space_type;
    // using V_space_type = FunctionSpaceDefinition::V_space_type;
    // using Vh_element_type = typename V_space_type::element_type;
    using q_sigma_space_type = space_type::template sub_functionspace<0>::type;
    using q_sigma_element_type = q_sigma_space_type::element_type;
    using V_view_type = typename element_type::template sub_element_type<0>;
    using V_view_ptrtype = typename element_type::template sub_element_ptrtype<0>;
    using T_view_ptrtype = typename element_type::template sub_element_ptrtype<1>;

    using current_space_type = FunctionSpace<mesh_type, bases<Lagrange<1, Vectorial> > >;
    using current_element_type = typename current_space_type::element_type;

    using prop_type = ModelProperties;
    using prop_ptrtype = boost::shared_ptr<prop_type>;

    using parameter_type = super_type::parameter_type;
    using vectorN_type = super_type::vectorN_type;
    using beta_vector_type = typename super_type::beta_vector_type;
    using beta_type = boost::tuple<beta_vector_type,  std::vector<beta_vector_type> >;
    using affine_decomposition_type = typename super_type::affine_decomposition_type;

    using sparse_matrix_ptrtype = typename super_type::sparse_matrix_ptrtype;

    using mdeim_type = MDEIM<self_type>;
    using mdeim_ptrtype = boost::shared_ptr<mdeim_type>;
    using deim_type = DEIM<self_type>;
    using deim_ptrtype = boost::shared_ptr<deim_type>;
    using mdeim_space_type = function_space_type::mdeim_space_type;
    using mdeim_space_ptrtype = function_space_type::mdeim_space_ptrtype;

private:
    mesh_ptrtype M_mesh;
    prop_ptrtype M_modelProps;
    std::vector< std::vector< element_ptrtype > > M_InitialGuess;

    element_ptrtype pT;
    V_view_ptrtype M_V;
    T_view_ptrtype M_T;
    parameter_type M_mu;

    double M_sigma;
    double M_k;
    double M_potDif;
    double M_h;
    double M_Tw;

    mdeim_ptrtype M_mdeim;
    deim_ptrtype M_deim;

public:
    // Constructors
    AlphaThermoelectric();
    AlphaThermoelectric( mesh_ptrtype mesh );

    // Helpers
    int Qa();
    int Nl();
    int Ql( int l );
    int mQA( int q );
    int mLQF( int l, int q );
    int mCompliantQ( int q );
    void resizeQm( bool resizeMat = true );
    parameter_type newParameter() { return Dmu->element(); }

    std::string alpha( parameter_type const& mu );
    std::string alphaPrime( parameter_type const& mu );
    sparse_matrix_ptrtype assembleForMDEIM( parameter_type const& mu ) override;
    vector_ptrtype assembleForDEIMnl( parameter_type const& mu );
    vector_ptrtype assembleForDEIMnl( parameter_type const& mu, element_type const& U );
    void initModel() override;
    void setupSpecificityModel( boost::property_tree::ptree const& ptree, std::string const& dbDir ) override;

    void decomposition();

    beta_vector_type computeBetaInitialGuess( parameter_type const& mu ) override;
    beta_type computeBetaQm( element_type const& T, parameter_type const& mu ) override;
    beta_type computeBetaQm( vectorN_type const& urb, parameter_type const& mu ) override;
    beta_type computeBetaQm( parameter_type const& mu ) override;
    beta_vector_type computeBetaLinearDecompositionA( parameter_type const& mu, double time=1e30 ) override;
    void fillBetaQm( parameter_type const& mu, vectorN_type betaA, vectorN_type betaF );

    affine_decomposition_type computeAffineDecomposition() override;
    std::vector<std::vector<sparse_matrix_ptrtype> > computeLinearDecompositionA() override;
    std::vector<std::vector<element_ptrtype> > computeInitialGuessAffineDecomposition() override;

    element_type solve( parameter_type const& mu ) override;
    value_type
    output( int output_index, parameter_type const& mu , element_type& u, bool need_to_solve=false);

    double sigma();
    void computeTruthCurrentDensity( current_element_type& j, parameter_type const& mu );
}; // AlphaThermoelectric class

} // namespace Feel

#endif