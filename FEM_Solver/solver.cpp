#include "solver.h"

MatrixXf solver::modify_master_stiffness()
{
	return MatrixXf();
}

Matrix2f solver::make_elem_stiffness(Vector2f& n1xy, Vector2f& n2xy, float E, float A)
{
	auto x21 = n2xy[0] - n1xy[0];
	auto y21 = n2xy[1] - n1xy[1];
	auto L = sqrtf(x21*x21 + y21*y21);
	auto c = x21 / L*1.0f;
	auto s = y21 / L*1.0f;
	auto cc = c*c;
	auto ss = s*s;
	auto cs = c*s;
	Matrix2f m;
	m << cc,  cs, -cc, -cs,
		 cc,  cs, -cc, -cs,
		-cc, -cs,  cc,  cs,
		-cs, -ss,  cs,  ss;
	return (E*A/L)*m; 
}

MatrixXf solver::assemble_master_stiffness()
{
	auto numnod = _mesh.nodesxy.size()/2;
	auto numele = _mesh.elements.size()/2;
	MatrixXf K_master(numnod * 2, numnod * 2);
	for (size_t t = 0; t < numele; ++t)
	{	
		auto ni = _mesh.elements(t);
		auto nj = _mesh.elements(t + 1);
		auto eft = solver_auxilary::getEFT(ni, nj);
		auto E = _mesh.props.mesh_material(t);
		auto A = _mesh.props.mesh_fabrication(t);

		auto K_ele = make_elem_stiffness(
			Vector2f(_mesh.nodesxy(ni), _mesh.nodesxy(ni+1)), 
			Vector2f(_mesh.nodesxy(nj), _mesh.nodesxy(nj+1)), 
			E, A);
		for (auto i = 0; i < 4; ++i) {
			auto ii = eft[i];
			for (auto j = 0; j < 4; ++j) {
				auto jj = eft[j];
				K_master(ii, jj) += K_ele(i, j);
				K_master(jj, ii) = K_master(ii, jj);
			}
		}

	}
	return K_master;
}

MatrixXf solver::modify_master_stiffness_if_zeros_constraint(MatrixXf & K_master)
{
	auto K_mod = K_master;
	assert(_mesh.forces_tag.size() == _mesh.node_forces.size());
	auto ftag_size = _mesh.forces_tag.size();
	for (size_t i = 0; i < ftag_size; ++i) {
		auto enum_val = static_cast<force_type>(_mesh.forces_tag(i));
		if (enum_val == force_type::UNKNOWN) {
			K_mod.row(i).setZero();
			K_mod.col(i).setZero();
			K_mod(i, i) = 1;
		}
	}
	return K_mod;
}

//TODO: Mixed matrices size error
VectorXf solver::retrieve_internal_forces(VectorXf& u)
{
	auto numele = _mesh.elements.size() / 2;
	VectorXf internal_forces(numele);
	for (auto e = 0; e < numele; ++e) {
		auto ni = _mesh.elements(e);
		auto nj = _mesh.elements(e + 1);
		auto E = _mesh.props.mesh_material(e);
		auto A = _mesh.props.mesh_fabrication(e);
		Vector4f ue;
		ue << u(ni * 2), u(2 * ni + 1), u(nj * 2), u(nj * 2 + 1);
		internal_forces(e) = calc_elem_internal_force(
			Vector2f(_mesh.nodesxy(ni), _mesh.nodesxy(ni + 1)),
			Vector2f(_mesh.nodesxy(nj), _mesh.nodesxy(nj + 1)),
			E, A, ue);
	}
	return internal_forces;
}

VectorXf solver::retrieve_internal_stresses(VectorXf &internal_forces)
{
	return internal_forces.cwiseQuotient(_mesh.props.mesh_fabrication);
}

float solver::calc_elem_internal_force(Vector2f& n1xy, Vector2f& n2xy, float E, float A, Vector4f& ue)
{
	auto x21 = n2xy(0) - n1xy(0);
	auto y21 = n2xy(1) - n1xy(1);
	auto LLe = x21*x21 + y21*y21;
	auto dux = ue(2) - ue(0);
	auto duy = ue(3) - ue(1);
	return (E*A/LLe)*(x21*dux + y21*duy);
}
