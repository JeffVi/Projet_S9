#include "voronoi_diagram.h"

cell::cell() : px(0.0f), py(0.0f) {}

cell::cell(float x, float y) : px(x), py(y) {}

int cell::get_id_vertex_origin(int k)
{
	int id;
	
	switch(k)
	{
		// haut droite
		case 1:
		id = id_vertex[1];
		break;
		
		// bas droite	
		case 3:
		id = id_vertex[2];
		break;
		
		// bas gauche
		case 5:
		id = id_vertex[3];
		break;
		
		// haut droite	
		case 7:
		id = id_vertex[0];
		break;
	}
	
	if(id == -1){std::cout<<"cross not solved !"<<std::endl;}
	return id;
}

int cell::get_update_id_vertex_origin(int k)
{
	int id;
	
	switch(k)
	{
		// haut droite
		case 1:
		id = id_vertex[1];
		id_vertex[1] = -1;
		id_vertex[2]++;
		id_vertex[3]++;
		break;
		
		// bas droite	
		case 3:
		id = id_vertex[2];
		id_vertex[2] = -1;
		id_vertex[3]++;
		break;
		
		// bas gauche
		case 5:
		id = id_vertex[3];
		id_vertex[3] = -1;
		break;
		
		// haut droite	
		case 7:
		id = id_vertex[0];
		id_vertex[0] = -1;
		id_vertex[1]++;
		id_vertex[2]++;
		id_vertex[3]++;
		break;
	}
	
	if(id == -1){std::cout<<"vertex already changed !"<<std::endl;}
	return id;
}

voronoi::voronoi(){}

voronoi::voronoi(Mat image ,Mat pixels_nodes, float diagram_scale)
{
	rows = image.rows;
    	cols = image.cols;
	imagep = image;
	pixels_nodes_todo = pixels_nodes.clone();
	scale = diagram_scale;
}

void voronoi::init_cells()
{
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			cell c(i*scale + scale/2 , j*scale + scale/2);
			c.vertex.push_back(Point(j*scale,i*scale)); 		// haut gauche
			c.vertex.push_back(Point((j+1)*scale,i*scale)); 	// haut droite
			c.vertex.push_back(Point((j+1)*scale,(i+1)*scale)); 	// bas droite
			c.vertex.push_back(Point(j*scale,(i+1)*scale)); 	// bas gauche
			c.color = imagep.at<Vec3b>(i,j);
			c.id_vertex.push_back(0);
			c.id_vertex.push_back(1);
			c.id_vertex.push_back(2);
			c.id_vertex.push_back(3);
			cells.push_back(c);
		}
	}
}

void voronoi::compute_vertex()
{
	Mat cells_pixels_nodes = pixels_nodes_todo.clone();
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			
			for(int k=0;k<4;k++)
			{
				if(cells_pixels_nodes.at<int>(i,j,2*k+1) == 1) //seulement les diagonales
				{
					std::vector<int> voisin = get_cell_adj(i,j,2*k+1);
					int vi = voisin[0];
					int vj = voisin[1];
					int vk = voisin[2];
					int vi_adj1 = voisin[3];
					int vj_adj1 = voisin[4];
					int vk_adj1 = voisin[5];
					int vi_adj2 = voisin[6];
					int vj_adj2 = voisin[7];
					int vk_adj2 = voisin[8];
					
					cells_pixels_nodes.at<int>(i,j,2*k+1) = 0;
					cells_pixels_nodes.at<int>(vi,vj,vk) = 0;
					
					//Cellule courante
					cell cell_cour = cells[i*cols + j];
					std::vector<Point> vertex = cell_cour.vertex;
					int vertex_id = cell_cour.get_update_id_vertex_origin(2*k+1);
					Point P1 = Point(cell_cour.py,cell_cour.px);
					
					//Cellule voisine
					cell cell_vois = cells[vi*cols + vj];
					std::vector<Point> vertex_voisin = cell_vois.vertex;
					int vertex_id_voisin = cell_vois.get_update_id_vertex_origin(vk);
					Point P2 = Point(cell_vois.py,cell_vois.px);
					
					//Cellule adjacente 1
					cell cell_adj1 = cells[vi_adj1*cols + vj_adj1];
					std::vector<Point> vertex_voisin_adj1 = cell_adj1.vertex;
					int vertex_id_voisin_adj1 = cell_adj1.get_id_vertex_origin(vk_adj1);
					Point P3 = Point(cell_adj1.py,cell_adj1.px);
					if(vertex_id_voisin_adj1==-1){break;}
					
					//Cellule adjacente 2
					cell cell_adj2 = cells[vi_adj2*cols + vj_adj2];
					std::vector<Point> vertex_voisin_adj2 = cell_adj2.vertex;
					int vertex_id_voisin_adj2 = cell_adj2.get_id_vertex_origin(vk_adj2);
					Point P4 = Point(cell_adj2.py,cell_adj2.px);
					if(vertex_id_voisin_adj2==-1){break;}
					
					//Compute vertex
					Point midpoint = P1 + ((P2 - P1)/2.0f);
					Point vertex1 = midpoint + ((P3 - midpoint)/2.0f);
					Point vertex2 = midpoint + ((P4 - midpoint)/2.0f);
					
					//Update vertex
					vertex[vertex_id] = vertex2;
					vertex.insert(vertex.begin() + vertex_id, vertex1);
					
					vertex_voisin[vertex_id_voisin] = vertex1;
					vertex_voisin.insert(vertex_voisin.begin() + vertex_id_voisin, vertex2);
					
					vertex_voisin_adj1[vertex_id_voisin_adj1] = vertex1;
					vertex_voisin_adj2[vertex_id_voisin_adj2] = vertex2;
					
					//Update cellule
					cell_cour.vertex = vertex;
					cell_vois.vertex = vertex_voisin;
					cell_adj1.vertex = vertex_voisin_adj1;
					cell_adj2.vertex = vertex_voisin_adj2;
					
					cells[i*cols + j] = cell_cour;
					cells[vi*cols + vj] = cell_vois;
					cells[vi_adj1*cols + vj_adj1] = cell_adj1;
					cells[vi_adj2*cols + vj_adj2] = cell_adj2;
				}
			}			
		}
	}
}

std::vector<int> voronoi::get_cell_adj(int i, int j, int k)
{
	//Cellule voisine en diagonale
	int vi;
	int vj;
	int vk;
	
	//Cellule adjacente 1
	int vi_adj1;
	int vj_adj1;
	int vk_adj1;
	
	//Cellule adjacente 2
	int vi_adj2;
	int vj_adj2;
	int vk_adj2;
	
	std::vector<int> voisins;
	
	switch(k)
	{
		case 1:
		vi = i-1;
		vj = j+1;
		vk = 5;
		vi_adj1 = i-1;
		vj_adj1 = j;
		vk_adj1 = 3;
		vi_adj2 = i;
		vj_adj2 = j+1;
		vk_adj2 = 7;
		break;
				
		case 3:
		vi = i+1;
		vj = j+1;
		vk = 7;
		vi_adj1 = i;
		vj_adj1 = j+1;
		vk_adj1 = 5;
		vi_adj2 = i+1;
		vj_adj2 = j;
		vk_adj2 = 1;
		break;
		
		case 5:
		vi = i+1;
		vj = j-1;
		vk = 1;
		vi_adj1 = i+1;
		vj_adj1 = j;
		vk_adj1 = 7;
		vi_adj2 = i;
		vj_adj2 = j-1;
		vk_adj2 = 3;
		break;
				
		case 7:
		vi = i-1;
		vj = j-1;
		vk = 3;
		vi_adj1 = i;
		vj_adj1 = j-1;
		vk_adj1 = 1;
		vi_adj2 = i-1;
		vj_adj2 = j;
		vk_adj2 = 5;
		break;
	}
	
	voisins.push_back(vi);
	voisins.push_back(vj);
	voisins.push_back(vk);
	voisins.push_back(vi_adj1);
	voisins.push_back(vj_adj1);
	voisins.push_back(vk_adj1);
	voisins.push_back(vi_adj2);
	voisins.push_back(vj_adj2);
	voisins.push_back(vk_adj2);
	
	return voisins;
}

Mat voronoi::draw_voronoi()
{
	Mat voro = Mat(rows*ceil(scale),cols*ceil(scale),CV_8UC3,Scalar(0));
	std::vector<cell>::iterator it;
	
	for(it=cells.begin(); it!=cells.end(); it++)
	{
		cell cellule = *it;
		std::vector<Point> vertex_list = cellule.vertex;
		const int npt = vertex_list.size();
		const Point* ppt = &vertex_list[0];
		fillPoly(voro,&ppt,&npt,1,cellule.color);
	}
		
	return voro;
}

void voronoi::draw_cells(Mat& voro)
{
	std::vector<cell>::iterator it;
	voro = voro + Scalar(100,100,100);//pour mieux voir les lignes noires
	
	for(it=cells.begin(); it!=cells.end(); it++)
	{
		cell cellule = *it;
		std::vector<Point> vertex_list = cellule.vertex;
		const int npt = vertex_list.size();
		const Point* ppt = &vertex_list[0];
		polylines(voro,&ppt,&npt,1,true,Scalar(0,0,0));
	}
}

Mat voronoi::draw_cells_union()
{
	Mat voro = Mat(rows*ceil(scale),cols*ceil(scale),CV_8UC3,Scalar(255,255,255));
	
	std::vector<cell>::iterator it;
	Scalar color_union = Scalar(0,0,0);
	
	for(it=polygons.begin(); it!=polygons.end(); it++)
	{
		color_union = color_union + Scalar(5,5,5); //max 51 cellules
		cell poly = *it;
		std::vector<Point> vertex_list = poly.vertex;
		const int npt = vertex_list.size();
		const Point* ppt = &vertex_list[0];
		fillPoly(voro,&ppt,&npt,1,color_union);
	}
	return voro;
}

void voronoi::draw_cells_union(Mat& voro)
{
	std::vector<cell>::iterator it;
	voro = voro + Scalar(100,100,100);//pour mieux voir les lignes noires
	
	for(it=polygons.begin(); it!=polygons.end(); it++)
	{
		
		cell poly = *it;
		std::vector<Point> vertex_list = poly.vertex;
		const int npt = vertex_list.size();
		const Point* ppt = &vertex_list[0];
		polylines(voro,&ppt,&npt,1,true,Scalar(0,0,0));
	}
}

void voronoi::draw_cells_union(Mat& voro, int id_cell)
{
	voro = voro + Scalar(100,100,100);//pour mieux voir les lignes noires
	
	cell poly = polygons[id_cell];
	std::vector<Point> vertex_list = poly.vertex;
	const int npt = vertex_list.size();
	const Point* ppt = &vertex_list[0];
	fillPoly(voro,&ppt,&npt,1,Scalar(0,0,0));
}

void voronoi::print_cell(cell cellule)
{
	std::vector<Point>::iterator it_vertcell;
	for (it_vertcell = cellule.vertex.begin(); it_vertcell != cellule.vertex.end(); it_vertcell++)
	{
		std::cout<<*it_vertcell<<std::endl;
	}
}

void voronoi::polygon()
{
	std::vector<cell>::iterator it;
	for (it = cells.begin(); it != cells.end(); it++)
	{
		bool segment = have_segment(*it);
		if (!segment)
		{
			cell T_cell = *it;
			polygons.push_back(T_cell);
		}

	}

}

void voronoi::polygon(int nbr_cells)
{
	int cpt = 0;
	std::vector<cell>::iterator it;
	for (it = cells.begin(); it != cells.end(); it++)
	{
		bool segment = have_segment(*it);
		if (!segment)
		{
			cell T_cell = *it;
			polygons.push_back(T_cell);
		}
	cpt++;
	if(cpt == nbr_cells){break;}
	}

}

bool voronoi::have_segment(cell cellule )
{

	std::vector<cell>::iterator it_poly;

	if (polygons.size() == 0)return false;

	bool have_segment = false;

	for (it_poly = polygons.begin(); it_poly != polygons.end(); it_poly++)
	{
		std::vector<Point>::iterator it_vertcell;
		
		cell cell_poly = *it_poly;

		bool same_color = compare_color(cell_poly.color,cellule.color);

		if (same_color) {
		
			for (it_vertcell = cellule.vertex.begin(); it_vertcell != cellule.vertex.end(); it_vertcell++)
			{
				std::vector<Point>::iterator it_vertpoly;
				int indice_vertpoly = -1;
				
				for (it_vertpoly = cell_poly.vertex.begin(); it_vertpoly != cell_poly.vertex.end(); it_vertpoly++)
				{

					indice_vertpoly++;
					
					int nbr_edges = find_edge(it_vertpoly,it_vertcell,cell_poly,cellule,indice_vertpoly);
					//cellule : de droite a gauche
					//poly : de gauche a droite
					
					//it_vertcell -> début du segment cellule
					//it_vertpoly -> fin du segment poly
					//it_p1 -> fin du segment cellule + 1
					//it_p2 -> début du segment poly
					
					if(nbr_edges >= 1)
					{
						have_segment = true;
						int indice_it_p2 = indice_vertpoly - nbr_edges + 1;
						int nbr_ajouts = 0;
						while(it_p1 != it_vertcell)
						{
							//std::cout<<"it_vertpoly avant ajout : "<<*it_vertpoly<<std::endl;
							
							cell_poly.vertex.insert(it_vertpoly,*it_p1);
							nbr_ajouts++;
							
							//std::cout<<"it_p1 ajouté : "<<*it_p1<<std::endl;
							
							indice_vertpoly++;
							it_vertpoly = cell_poly.vertex.begin() + indice_vertpoly;
							
							//std::cout<<"it_vertpoly apres ajout : "<<*it_vertpoly<<std::endl;
							
							if (it_p1 != cellule.vertex.end()-1)
							{
								it_p1++;
							}
							else
							{
								it_p1 = cellule.vertex.begin();
							}
						}
						
						//std::cout<<"cell apres ajouts : "<<std::endl;
						//print_cell(cell_poly);
						
						if(nbr_edges > 1)
						{
							//Point p_sup1 = *(cell_poly.vertex.begin() + indice_it_p2);
							//Point p_sup2 = *(cell_poly.vertex.begin() + indice_vertpoly - nbr_ajouts);
							//std::cout<<"vertpoly supprimé début : "<<p_sup1<<std::endl;
							//std::cout<<"vertpoly supprimé fin : "<<p_sup2<<std::endl;
							
							cell_poly.vertex.erase(cell_poly.vertex.begin() + indice_it_p2, cell_poly.vertex.begin() + indice_vertpoly - nbr_ajouts);
							
							//std::cout<<"cell fin iter : "<<std::endl;
							//print_cell(cell_poly);
						}
					}
					
					*it_poly = cell_poly;
					if(have_segment){return true;}
				}
			}
		}
	}

	return have_segment;

}

int voronoi::find_edge(std::vector<Point>::iterator& it_vertpoly, std::vector<Point>::iterator& it_vertcell, cell& poly, cell& cellule, int& indice_vertpoly)
{
	int cpt = -1;
	it_p1 = it_vertcell;
	it_p2 = it_vertpoly;
	double dP = norm(*it_p1 - *it_p2);
	float seuil = 0.05f;
	if(dP >= seuil){return cpt;}
	
	//std::cout<<std::endl<<std::endl;
	//std::cout<<"it_vertcell : "<<*it_vertcell<<std::endl;
	//std::cout<<"it_vertpoly : "<<*it_vertpoly<<std::endl;
	
	//Sens trigo
	while(dP < seuil)
	{
		if (it_p2 != poly.vertex.end()-1)
		{
			it_p2++;
		}
		else
		{
			it_p2 = poly.vertex.begin();
		}

		if (it_p1 != cellule.vertex.begin())
		{
			it_p1--;
		}
		else
		{
			it_p1 = cellule.vertex.end()-1;
		}
		
		dP = norm(*it_p1 - *it_p2);
		indice_vertpoly++;
		
		//std::cout<<"boucle trigo"<<std::endl;
		//std::cout<<"it_p1 : "<<*it_p1<<std::endl;
		//std::cout<<"it_p2 : "<<*it_p2<<std::endl;
	}
	
	if (it_p1 != cellule.vertex.end()-1)
	{
		it_p1++;
	}
	else
	{
		it_p1 = cellule.vertex.begin();
	}

	if (it_p2 != poly.vertex.begin())
	{
		it_p2--;
	}
	else
	{
		it_p2 = poly.vertex.end()-1;
	}
	
	indice_vertpoly--;
	it_vertcell = it_p1;
	it_vertpoly = it_p2;
	dP = norm(*it_p1 - *it_p2);
		
	//Sens horaire
	while(dP < seuil)
	{
		if (it_p1 != cellule.vertex.end()-1)
		{
			it_p1++;
		}
		else
		{
			it_p1 = cellule.vertex.begin();
		}

		if (it_p2 != poly.vertex.begin())
		{
			it_p2--;
		}
		else
		{
			it_p2 = poly.vertex.end()-1;
		}
		
		dP = norm(*it_p1 - *it_p2);
		cpt++;
		
		//std::cout<<"boucle horaire"<<std::endl;
		//std::cout<<"it_p1 : "<<*it_p1<<std::endl;
		//std::cout<<"it_p2 : "<<*it_p2<<std::endl;
	}
	
	if (it_p2 != poly.vertex.end()-1)
	{
		it_p2++;
	}
	else
	{
		it_p2 = poly.vertex.begin();
	}
	
	//if(cpt>0){std::cout<<"nbr segments : "<<cpt<<std::endl;}
	
	//std::cout<<"it_p1 fin : "<<*it_p1<<std::endl;
	//std::cout<<"it_p2 fin : "<<*it_p2<<std::endl;
	
	//std::cout<<"it_vertcell fin : "<<*it_vertcell<<std::endl;
	//std::cout<<"it_vertpoly fin : "<<*it_vertpoly<<std::endl;
	
	return cpt;
}

bool voronoi::compare_color(const Scalar& color1, const Scalar& color2)
{

	bool dB;
	bool dG;
	bool dR;
	
	int R1 = color1[2];
	int G1 = color1[1];
	int B1 = color1[0];
	int R2 = color2[2];
	int G2 = color2[1];
	int B2 = color2[0];
	
	dR = abs(R1-R2)<20;
	dG = abs(G1-G2)<20;
	dB = abs(B1-B2)<20;
	
	return (dR && dG && dB);

}

void voronoi::union_poly()
{
	std::vector<cell>::iterator it_poly_1;
	std::vector<cell>::iterator it_poly_2;
	int indice_poly_1 = -1;
	bool edge_found = false;
	
	for(it_poly_1 = polygons.begin(); it_poly_1 != polygons.end(); it_poly_1++)
	{
		cell poly_1 = *it_poly_1;
		indice_poly_1++;
		int indice_poly_2 = indice_poly_1;
		
		//std::cout<<"*****indice_poly_1 : "<<indice_poly_1<<std::endl;
		
		for(it_poly_2 = polygons.begin() + indice_poly_1 + 1; it_poly_2 != polygons.end(); it_poly_2++)
		{
			cell poly_2 = *it_poly_2;
			indice_poly_2++;
			
			//std::cout<<"indice_poly_2 : "<<indice_poly_2<<std::endl;
			
			edge_found = false;
			
			if(compare_color(poly_1.color, poly_2.color))
			{
				std::vector<Point>::iterator it_vertpoly_1;
				std::vector<Point>::iterator it_vertpoly_2;
				int indice_vertpoly_1 = -1;
				
				for(it_vertpoly_1 = poly_1.vertex.begin(); it_vertpoly_1 != poly_1.vertex.end(); it_vertpoly_1++)
				{
					indice_vertpoly_1++;
					for(it_vertpoly_2 = poly_2.vertex.begin(); it_vertpoly_2 != poly_2.vertex.end(); it_vertpoly_2++)
					{
						int nbr_edges = find_edge(it_vertpoly_1,it_vertpoly_2,poly_1,poly_2,indice_vertpoly_1);
						
						if(nbr_edges >= 1)
						{
							//std::cout<<"###edge found !"<<std::endl;
							
							edge_found = true;
							int indice_it_p2 = indice_vertpoly_1 - nbr_edges + 1;
							int nbr_ajouts = 0;
							while(it_p1 != it_vertpoly_2)
							{
								//std::cout<<"it_vertpoly avant ajout : "<<*it_vertpoly_1<<std::endl;
								
								poly_1.vertex.insert(it_vertpoly_1,*it_p1);
								nbr_ajouts++;
								
								indice_vertpoly_1++;
								it_vertpoly_1 = poly_1.vertex.begin() + indice_vertpoly_1;
								
								if (it_p1 != poly_2.vertex.end()-1)
								{
									it_p1++;
								}
								else
								{
									it_p1 = poly_2.vertex.begin();
								}
							}
							
							//std::cout<<"cell apres ajouts : "<<std::endl;
							//print_cell(poly_1);
							
							if(nbr_edges > 1)
							{
								poly_1.vertex.erase(poly_1.vertex.begin() + indice_it_p2, poly_1.vertex.begin() + indice_vertpoly_1 - nbr_ajouts);
								
							}
							
							*it_poly_1 = poly_1;
							polygons.erase(it_poly_2);
							it_poly_1 = polygons.begin() + indice_poly_1;
							it_poly_2 = polygons.begin() + indice_poly_1;
							indice_poly_2 = indice_poly_1;
							
							//std::cout<<"nombre polys : "<<polygons.size()<<std::endl;
							
							break;
						}
					}
					if(edge_found){break;}
				}
			}
		}
	}
}
