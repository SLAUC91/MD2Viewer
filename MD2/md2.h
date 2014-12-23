#ifndef		__MD2_H__
#define		__MD2_H__

#include <GL/freeglut.h>
#include <fstream>
#include "Point.h"

// magic number - IDP2
#define MD2_ID (('2' << 24) + ('P' << 16) + ('D' << 8) + 'I')

// model version
#define	MD2_VERSION	8

// maximum number of vertices for a MD2 model
#define MAX_MD2_VERTS 2048

typedef float vec3_t[3];

// MD2 Header
typedef struct
{
	int		id;					// magic number. must be equal to "IPD2"
	int		version;			// md2 version. must be equal to 8

	int		skinwidth;			// width of the texture
	int		skinheight;			// height of the texture
	int		framesize;			// size of one frame in bytes

	int		num_skins;			// number of textures
	int		num_xyz;			// number of vertices
	int		num_st;				// number of texture coordinates
	int		num_tris;			// number of triangles
	int		num_glcmds;			// number of opengl commands
	int		num_frames;			// total number of frames

	int		ofs_skins;			// offset to skin names (64 bytes each)
	int		ofs_st;				// offset to s-t texture coordinates
	int		ofs_tris;			// offset to triangles
	int		ofs_frames;			// offset to frame data
	int		ofs_glcmds;			// offset to opengl commands
	int		ofs_end;			// offset to the end of file
} md2_t;

typedef struct
{
	short u, v;
} tex_coord;

// Triangle
typedef struct
{
	short   index_xyz[3];    // indexes to triangle's vertices
	short   index_st[3];     // indexes to vertices' texture coorinates
} triangle_t;

// Vertex
typedef struct
{
	unsigned char	v[3];				// compressed vertex' (x, y, z) coordinates
	unsigned char	lightnormalindex;	// index to a normal vector for the lighting
} vertex_t;

// Frame
typedef struct
{
	float		scale[3];		// scale values
	float		translate[3];	// translation vector
	char		name[16];		// frame name
	vertex_t	verts[1];		// first vertex of this frame
} frame_t;

// Animation
typedef struct
{
	int		first_frame;			// first frame of the animation
	int		last_frame;				// number of frames
	int		fps;					// number of frames per second
} anim_t;

// Animation List
typedef enum {
	STAND,
	RUN,
	ATTACK,
	PAIN_A,
	PAIN_B,
	PAIN_C,
	JUMP,
	FLIP,
	SALUTE,
	FALLBACK,
	WAVE,
	_POINT, //redef
	CROUCH_STAND,
	CROUCH_WALK,
	CROUCH_ATTACK,
	CROUCH_PAIN,
	CROUCH_DEATH,
	DEATH_FALLBACK,
	DEATH_FALLFORWARD,
	DEATH_FALLBACKSLOW,
	BOOM,
	MAX_ANIMATIONS
} animType_t;

// CMD2Model - MD2 model class object
class MD2
{
private:
	int				num_frames;			// number of frames
	int				num_xyz;			// number of vertices
	int				num_tris;			// number of triangles
	int				num_glcmds;			// number of glcommands
	int				skin_width;
	int				skin_height;

	vec3_t			*m_vertices;		// vertex array
	tex_coord		*texs;				// textures 
	triangle_t		*tris;				// triangles

	static anim_t animlist[21];			// animation list

public:
	MD2(void);
	virtual ~MD2(void);

	bool LoadModel(const char *filename);
	void frame(int animate, int wireframe, Point scale, Point tran, int shading);
	void interpol(vec3_t *vertex_list, Point scale, Point tran, int type);
	float *	calNormal(float *a, float *b, float *c);
	float * calNormal1(float *a, vec3_t *vertex_list);
};

#endif