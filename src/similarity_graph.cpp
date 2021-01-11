#include "similarity_graph.h"

similarity_graph::similarity_graph(){}

similarity_graph::similarity_graph(Mat image)
{
	rows = image.rows;
    	cols = image.cols;
	
	int sizes[3];
	sizes[0] = rows;
	sizes[1] = cols;
	sizes[2] = 8;
	
	int z = 0;
	Mat m(3,sizes,CV_32SC1,z);
	pixels_nodes = m;
	
	cvtColor(image,image_YUV,COLOR_BGR2YUV);
}

void similarity_graph::init_graph()
{
	//Connexion pour les pixels ayant 8 voisins.
	for(int i=1;i<rows-1;i++)
	{
		for(int j=1;j<cols-1;j++)
		{
			for(int k=0;k<8;k++)
			{
				pixels_nodes.at<int>(i,j,k) = 1;
			}
		}
	}
	
	//Connexion pour les pixels des bords.
	for(int j=1;j<cols-2;j++)
	{
		//Haut
		pixels_nodes.at<int>(0,j,2) = 1;
		pixels_nodes.at<int>(0,j,3) = 1;
		pixels_nodes.at<int>(0,j,4) = 1;
		pixels_nodes.at<int>(0,j,5) = 1;
		pixels_nodes.at<int>(0,j,6) = 1;
		
		//Bas
		pixels_nodes.at<int>(rows-1,j,6) = 1;
		pixels_nodes.at<int>(rows-1,j,7) = 1;
		pixels_nodes.at<int>(rows-1,j,0) = 1;
		pixels_nodes.at<int>(rows-1,j,1) = 1;
		pixels_nodes.at<int>(rows-1,j,2) = 1;
	}
	
	for(int i=1;i<rows-2;i++)
	{
		//Droite
		pixels_nodes.at<int>(i,cols-1,0) = 1;
		pixels_nodes.at<int>(i,cols-1,7) = 1;
		pixels_nodes.at<int>(i,cols-1,6) = 1;
		pixels_nodes.at<int>(i,cols-1,5) = 1;
		pixels_nodes.at<int>(i,cols-1,4) = 1;
		
		//Gauche
		pixels_nodes.at<int>(i,0,0) = 1;
		pixels_nodes.at<int>(i,0,1) = 1;
		pixels_nodes.at<int>(i,0,2) = 1;
		pixels_nodes.at<int>(i,0,3) = 1;
		pixels_nodes.at<int>(i,0,4) = 1;
	}
	
	//Connexion pour les pixels des coins.
	pixels_nodes.at<int>(0,0,2) = 1;
	pixels_nodes.at<int>(0,0,3) = 1;
	pixels_nodes.at<int>(0,0,4) = 1;
	
	pixels_nodes.at<int>(rows-1,0,0) = 1;
	pixels_nodes.at<int>(rows-1,0,1) = 1;
	pixels_nodes.at<int>(rows-1,0,2) = 1;
	
	pixels_nodes.at<int>(0,cols-1,4) = 1;
	pixels_nodes.at<int>(0,cols-1,5) = 1;
	pixels_nodes.at<int>(0,cols-1,6) = 1;
	
	pixels_nodes.at<int>(rows-1,cols-1,0) = 1;
	pixels_nodes.at<int>(rows-1,cols-1,7) = 1;
	pixels_nodes.at<int>(rows-1,cols-1,6) = 1;
	
}

void similarity_graph::compare_YUV()
{
	bool dY;
	bool dU;
	bool dV;
	
	int vi;
	int vj; 
	
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			for(int k=0;k<8;k++)
			{
				if(pixels_nodes.at<int>(i,j,k) == 1)
				{
					int Y = image_YUV.at<Vec3b>(i,j)[0];
					int U = image_YUV.at<Vec3b>(i,j)[1];
					int V = image_YUV.at<Vec3b>(i,j)[2];
			
					auto voisin = get_voisin(i,j,k);
					int vi = std::get<0>(voisin);
					int vj = std::get<1>(voisin);
					int vk = std::get<2>(voisin);
					
					dY = abs(Y - image_YUV.at<Vec3b>(vi,vj)[0])>48;
					dU = abs(U - image_YUV.at<Vec3b>(vi,vj)[1])>7;
					dV = abs(V - image_YUV.at<Vec3b>(vi,vj)[2])>6;
					
					if(dY || dU || dV)
					{
						pixels_nodes.at<int>(i,j,k) = 0;
						pixels_nodes.at<int>(vi,vj,vk) = 0;
					}
				}
			}
		}
	}
}

std::tuple<int, int, int> similarity_graph::get_voisin(int i, int j, int k)
{
	int vi;
	int vj;
	int vk;
	
	switch(k)
	{
		case 0:
		vi = i-1;
		vj = j;
		vk = 4;
		break;
		
		case 1:
		vi = i-1;
		vj = j+1;
		vk = 5;
		break;
		
		case 2:
		vi = i;
		vj = j+1;
		vk = 6;
		break;
		
		case 3:
		vi = i+1;
		vj = j+1;
		vk = 7;
		break;
		
		case 4:
		vi = i+1;
		vj = j;
		vk = 0;
		break;
		
		case 5:
		vi = i+1;
		vj = j-1;
		vk = 1;
		break;
		
		case 6:
		vi = i;
		vj = j-1;
		vk = 2;
		break;
		
		case 7:
		vi = i-1;
		vj = j-1;
		vk = 3;
		break;
	}
	
	return std::make_tuple(vi,vj,vk);
}
