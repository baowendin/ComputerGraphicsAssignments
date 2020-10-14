#include "sphere.h"
void Sphere::paint()
{
	material->glSetMaterial();
	vector<Vec3f> points;
	float unit_theta = M_PI * 2 / theta_num, unit_phi = M_PI / phi_num;
	//生成phi_num * theta_num个点
	for (int phi = 0; phi < phi_num + 1; phi++)
		for (int theta = 0; theta < theta_num; theta++)
		{
			float phi_val = -M_PI_2 + unit_phi * phi;
			float theta_val = unit_theta * theta;
			points.push_back(
				Vec3f(radius * cos(phi_val) * sin(theta_val),
					radius * cos(phi_val) * cos(theta_val),
					radius * sin(phi_val)
				) + center);
		}
	// 层序遍历用这些点画四边形
	int count = 0;
	glBegin(GL_QUADS);
	for (int phi = 0; phi < phi_num; phi++)
	{
		Vec3f p[4];
		for (int theta = 0; theta < theta_num; theta++)
		{
			p[0] = points[phi * theta_num + theta];
			p[1] = points[phi * theta_num + (theta + 1) % theta_num];
			p[2] = points[(phi + 1) * theta_num + (theta + 1) % theta_num];
			p[3] = points[(phi + 1) * theta_num + theta];
			Vec3f normal;
			if (phi == phi_num - 1)
			{
				Vec3f::Cross3(normal, p[3] - p[0], p[1] - p[3]);
			}
			else
			{
				Vec3f::Cross3(normal, p[3] - p[0], p[2] - p[3]);
			}
			normal.Normalize();
			for (int i = 0; i < 4; i++)
			{
				if (gouraud_used)
				{
					normal = p[i] - center;
					normal.Normalize();
				}
				glNormal3f(normal.x(), normal.y(), normal.z());
				glVertex3f(p[i].x(), p[i].y(), p[i].z());
			}
			count++;
		}
		
	}
	glEnd();
}

void Sphere::insertIntoGrid(Grid* g, Matrix* m)
{
	if (m)
		g->insert_matrix(this, new Matrix(*m));
	Vec3f trans_c = center;
	if (m)
	{
		m->Transform(trans_c);
	}
	for (int i = 0; i < g->nx; i++)
		for (int j = 0; j < g->ny; j++)
			for (int k = 0; k < g->nz; k++)
			{
				float center_dis = (trans_c - g->get_center(i, j, k)).Length();
				float dialog_dis = g->get_halfdialog();
				float trans_r = radius;
				//由于球不是透明的，所以我们仅仅需要处理球的外壳和cell相交
				if (m)
				{
					Vec3f dir = g->get_center(i, j, k) - trans_c;
					dir.Normalize();
					m->TransformDirection(dir);
					trans_r = radius * dir.Length();
				}
				if (center_dis - dialog_dis <= trans_r && center_dis + dialog_dis >= trans_r)
					g->addObject(i, j, k, this);
			}
}