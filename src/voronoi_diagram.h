#ifndef VORONOI_DIAGRAM_H
#define VORONOI_DIAGRAM_H

#include <opencv2/opencv.hpp>

using namespace cv;

class cell
{
public:

	cell();
	cell(float x, float y);
	cell(std::vector<Point>  T_vertex, Scalar T_color);

	float px;
	float py;

	std::vector<Point> vertex;
	Scalar color;
	std::vector<int> id_vertex;

	int get_id_vertex_origin(int k);
	int get_update_id_vertex_origin(int k);
};

class voronoi
{
public:

	float scale;
	int rows;
	int cols;
	
	voronoi();
	voronoi(Mat image, Mat pixels_nodes, float diagram_scale);
	
	std::vector<cell> polygons;
	
	void polygon();
	void polygon(int nbr_cells);
	void union_poly();
	bool have_segment(cell cellule);
	void init_cells();
	void compute_vertex();
	bool compare_color(const Scalar& color1, const Scalar& color2);
	Mat draw_voronoi();
	void draw_cells(Mat& voro);
	Mat draw_cells_union();
	void draw_cells_union(Mat& voro);
	void draw_cells_union(Mat& voro, int id_cell);
	void print_cell(cell cellule);

private:

	Mat imagep;
	Mat pixels_nodes_todo;
	std::vector<cell> cells;
	std::vector<Point>::iterator it_p1;
	std::vector<Point>::iterator it_p2;
	
	std::vector<int> get_cell_adj(int i, int j, int k);
	int find_edge(std::vector<Point>::iterator& it_vertpoly, std::vector<Point>::iterator& it_vertcell, cell& poly, cell& cellule, int& indice_vertpoly);
};

#endif
