#pragma once
#include <Eigen/Dense>
#include <cmath>
#include <type_traits>
#include <assert.h>

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
	return static_cast<std::underlying_type_t<E>>(e);
}

using namespace Eigen;
enum class force_type { KNOWN, UNKNOWN, SOLVED };
enum constraint {};

struct mesh_properties {
	VectorXf mesh_material;
	VectorXf mesh_fabrication;
};

struct mesh {
	bool isComplete = false;
	mesh_properties props; // mesh properties
	VectorXi elements; // record connected links of nodes
	VectorXf nodesxy; // record position of nodes in cartesian
	VectorXf node_forces; // record force(x-y) in each nodes
	VectorXi forces_tag;
	size_t get_number_of_nodes() {
		return nodesxy.size()/2;
	}
	
	size_t get_number_of_elements() {
		return elements.cols() == 2 ? elements.rows() : elements.cols();
	}
};

namespace solver_printer {
	/*print to console eigen matrices
	*/
	inline void writeToConsole() {

	}
}

namespace solver_auxilary{
	inline Vector4i getEFT(int ni, int nj) {
		return Vector4i(2 * ni, 2 * ni + 1, 2 * nj, 2 * nj + 1);
	}
}

class solver
{
private:
	MatrixXf modify_master_stiffness();
	float calc_elem_internal_force(Vector2f& n1xy, Vector2f& n2xy, float E, float A, Vector4f& ue);
	Matrix2f make_elem_stiffness(Vector2f& n1xy, Vector2f& n2xy, float E, float A);

public:
	MatrixXf assemble_master_stiffness();
	MatrixXf modify_master_stiffness_if_zeros_constraint(MatrixXf& K_master);
	VectorXf retrieve_internal_forces(VectorXf& u);
	VectorXf retrieve_internal_stresses(VectorXf &internal_forces);
	solver(mesh msh) : _mesh(msh) {};
	mesh _mesh;
};

