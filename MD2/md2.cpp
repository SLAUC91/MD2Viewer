#include "md2.h"

// Initialize the 21 MD2 model animations

anim_t MD2::animlist[21] =
{
	// first, last, fps
	{ 0, 39, 9 },	// STAND
	{ 40, 45, 10 },	// RUN
	{ 46, 53, 10 },	// ATTACK
	{ 54, 57, 7 },	// PAIN_A
	{ 58, 61, 7 },	// PAIN_B
	{ 62, 65, 7 },	// PAIN_C
	{ 66, 71, 7 },	// JUMP
	{ 72, 83, 7 },	// FLIP
	{ 84, 94, 7 },	// SALUTE
	{ 95, 111, 10 },	// FALLBACK
	{ 112, 122, 7 },	// WAVE
	{ 123, 134, 6 },	// POINT
	{ 135, 153, 10 },	// CROUCH_STAND
	{ 154, 159, 7 },	// CROUCH_WALK
	{ 160, 168, 10 },	// CROUCH_ATTACK
	{ 169, 172, 7 },	// CROUCH_PAIN
	{ 173, 177, 5 },	// CROUCH_DEATH
	{ 178, 183, 7 },	// DEATH_FALLBACK
	{ 184, 189, 7 },	// DEATH_FALLFORWARD
	{ 190, 197, 7 },	// DEATH_FALLBACKSLOW
	{ 198, 198, 5 },	// BOOM
};

MD2::MD2(void)
{
	m_vertices = 0;
	num_frames = 0;
	num_xyz = 0;
	num_tris = 0;
	texs = 0;
	tris = 0;
	num_glcmds = 0;
}

MD2::~MD2(void)
{
	if (m_vertices) delete[] m_vertices;
	if (texs) delete[] texs;
	if (tris) delete[] tris;
}

bool MD2::LoadModel(const char *filename)
{
	std::ifstream	file;			// file stream
	md2_t			header;			// md2 header
	char			*buffer;		// buffer storing frame data
	frame_t			*frame;			// temporary variable
	vec3_t			*ptrverts;		// pointer on m_vertices

	// try to open filename
	if (filename != NULL) { 
		file.open(filename, std::ios::in | std::ios::binary);
	}

	if (file.fail())
		return false;

	// read header file
	file.read((char *)&header, sizeof(md2_t));

	/////////////////////////////////////////////
	// verify that this is a MD2 file
	if ((header.id != MD2_ID) && (header.version != MD2_VERSION))
	{
		// this is not a MD2 model
		file.close();
		return false;
	}
	/////////////////////////////////////////////

	// initialize member variables
	num_frames = header.num_frames;
	num_xyz = header.num_xyz;
	num_tris = header.num_tris;
	skin_width = header.skinwidth;
	skin_height = header.skinheight;
	num_glcmds = header.num_glcmds;

	// allocate memory
	m_vertices = new vec3_t[num_xyz * num_frames];
	buffer = new char[num_frames * header.framesize];
	texs = new tex_coord[header.num_st];
	tris = new triangle_t[header.num_tris];

	/////////////////////////////////////////////
	// reading file data
	file.seekg(header.ofs_st, std::ios::beg);
	file.read((char *)texs, header.num_st*sizeof(tex_coord));

	// read tris info
	file.seekg(header.ofs_tris, std::ios::beg);
	file.read((char *)tris, header.num_tris*sizeof(triangle_t));

	// read frame data...
	file.seekg(header.ofs_frames, std::ios::beg);
	file.read((char *)buffer, num_frames * header.framesize);


	/////////////////////////////////////////////

	// vertex array initialization
	for (int j = 0; j < num_frames; j++)
	{
		// adjust pointers
		frame = (frame_t *)&buffer[header.framesize * j];
		ptrverts = &m_vertices[num_xyz * j];

		for (int i = 0; i < num_xyz; i++)
		{
			ptrverts[i][0] = (frame->verts[i].v[0] * frame->scale[0]) + frame->translate[0];
			ptrverts[i][1] = (frame->verts[i].v[1] * frame->scale[1]) + frame->translate[1];
			ptrverts[i][2] = (frame->verts[i].v[2] * frame->scale[2]) + frame->translate[2];
		}
	}

	// free buffer's memory
	delete[] buffer;

	// close the file and return
	file.close();
	return true;
}

//Calculate normal given 3 floats
float * MD2::calNormal(float *a, float *b, float *c){
	float VEC_1[3];
	float VEC_2[3];
	float VEC_R[3];
	float LENGTH_VEC;

	float * end = new float[3];
	end[0] = 0; end[1] = 0; end[2] = 0;

	//vectors 1 and 2
	VEC_1[0] = b[0] - a[0];
	VEC_1[1] = b[1] - a[1];
	VEC_1[2] = b[2] - a[2];

	VEC_2[0] = b[0] - c[0];
	VEC_2[1] = b[1] - c[1];
	VEC_2[2] = b[2] - c[2];

	//cross product
	VEC_R[0] = (VEC_1[1] * VEC_2[2]) - (VEC_2[1] * VEC_1[2]);
	VEC_R[1] = (VEC_2[0] * VEC_1[2]) - (VEC_1[0] * VEC_2[2]);
	VEC_R[2] = (VEC_1[0] * VEC_2[1]) - (VEC_2[0] * VEC_1[1]);

	//normal -> Unit vector
	LENGTH_VEC = sqrt((VEC_R[0] * VEC_R[0]) + (VEC_R[1] * VEC_R[1]) + (VEC_R[2] * VEC_R[2]));

	//glNormal3f(VEC_R[0]/LENGTH_VEC, VEC_R[1]/LENGTH_VEC, VEC_R[2]/LENGTH_VEC);
	end[0] = VEC_R[0] / LENGTH_VEC;
	end[1] = VEC_R[1] / LENGTH_VEC;
	end[2] = VEC_R[2] / LENGTH_VEC;

	return end;
}

//Calculate normal given point and vertex list
float * MD2::calNormal1(float *a, vec3_t *vertex_list){
	int n = 0;

	float * end = new float[3];
	end[0] = 0; end[1] = 0; end[2] = 0;

	for (int i = 0; i < num_tris; i++){
		for (int j = 0; j < 3; j++){
			if (a[0] == vertex_list[tris[i].index_xyz[j]][0]){
				if (a[1] == vertex_list[tris[i].index_xyz[j]][1]){
					if (a[2] == vertex_list[tris[i].index_xyz[j]][2]){
						float * Array = calNormal(vertex_list[tris[i].index_xyz[0]], vertex_list[tris[i].index_xyz[1]], vertex_list[tris[i].index_xyz[2]]);
						end[0] += Array[0];
						end[1] += Array[1];
						end[2] += Array[2];
						n += 1;
					}
				}
			}
		}
	}

	end[0] /= n;
	end[1] /= n;
	end[2] /= n;

	return end;
}

void MD2::interpol(vec3_t *vertex_list, Point scale, Point tran, int type){
	//animation
	int animation = animlist[type].first_frame;
	//int end_anim = animlist[type].last_frame;

	for (int j = 0; j < num_xyz; j++){
		vertex_list[j][0] = (m_vertices[j + (num_xyz*animation)][0])*scale.x() + tran.x();
		vertex_list[j][1] = (m_vertices[j + (num_xyz*animation)][1])*scale.y() + tran.y();
		vertex_list[j][2] = (m_vertices[j + (num_xyz*animation)][2])*scale.z() + (tran.z() - 2.57);
	}
}

void MD2::frame(int animate, int wireframe, Point scale, Point tran, int shading){
	static vec3_t vertex_list[MAX_MD2_VERTS];
	interpol(vertex_list, scale, tran, animate);

	if (shading == 0) {
		if (wireframe == 0) {
			for (int i = 0; i < num_tris; ++i){
				glBegin(GL_LINE_LOOP);
				for (int j = 0; j < 3; ++j){
					glColor3f(0.0, 0.7, 1.0);
					glNormal3fv(calNormal(vertex_list[tris[i].index_xyz[0]], vertex_list[tris[i].index_xyz[1]], vertex_list[tris[i].index_xyz[2]]));
					glVertex3fv(vertex_list[tris[i].index_xyz[j]]);
				}
				glEnd();
			}
		}
		else if (wireframe == 1) {
			for (int i = 0; i < num_tris; ++i){
				glBegin(GL_TRIANGLES);
				for (int j = 0; j < 3; ++j){
					glColor3f(0.0, 0.7, 1.0);
					glNormal3fv(calNormal(vertex_list[tris[i].index_xyz[0]], vertex_list[tris[i].index_xyz[1]], vertex_list[tris[i].index_xyz[2]]));
					glVertex3fv(vertex_list[tris[i].index_xyz[j]]);
				}
				glEnd();
			}
		}
	}
	else if (shading == 1) {
		if (wireframe == 0) {
			for (int i = 0; i < num_tris; ++i){
				glBegin(GL_LINE_LOOP);
				for (int j = 0; j < 3; ++j){
					glColor3f(0.0, 0.7, 1.0);
					glNormal3fv(calNormal(vertex_list[tris[i].index_xyz[0]], vertex_list[tris[i].index_xyz[1]], vertex_list[tris[i].index_xyz[2]]));
					glVertex3fv(vertex_list[tris[i].index_xyz[j]]);
				}
				glEnd();
			}
		}
		else if (wireframe == 1) {
			for (int i = 0; i < num_tris; ++i){
				glBegin(GL_TRIANGLES);
				for (int j = 0; j < 3; ++j){
					glColor3f(0.0, 0.7, 1.0);
					glNormal3fv(calNormal1(vertex_list[tris[i].index_xyz[j]], vertex_list));
					glVertex3fv(vertex_list[tris[i].index_xyz[j]]);
				}
				glEnd();
			}
		}

	}
}