#include "Angel.h" // include point types and operations

typedef vec4 point4; 

void circle( point4 vertices[], int sides, GLfloat radius, GLfloat z)
{
	GLfloat angle = 360.0/(GLfloat) sides;
	for (int i = 0; i < sides; i++)
	{
		vertices[i] = 	point4( radius* cos(angle * (GLfloat)i* M_PI/180.0), radius* sin(angle * (GLfloat)i* M_PI/180.0), z, 1.0 );
	}
}

void cylinder( point4 vertices[], int sides, GLfloat radius, GLfloat z, GLfloat height)
{
	circle( vertices, sides, radius, z);
	circle( &vertices[ sides ], sides, radius, z + height);
}

void cylinder_plane( int sides, GLubyte indice[])
{
	for (int i = 0 ; i < sides ; i++)
	{
		int index = i * 6;
		indice[index] = (i + 1) % sides ;
		indice[index + 1] = i ;
		indice[index + 2] = i + sides;
		indice[index + 3] = (i + 1) % sides;
		indice[index + 4] = i + sides;
		indice[index + 5] = (i + 1) % sides + sides;
	}
	int offset = sides * 6;
	for (int i = 0; i < sides - 2 ; i++)
	{
		indice[i*3 + offset] = 0 ;
		indice[i*3 + offset + 1] = i + 1 ;
		indice[i*3 + offset + 2] = i + 2 ;
	}

	offset = sides * 6 + (sides - 2) * 3;
	for (int i = 0; i < sides - 2; i++)
	{
		indice[i*3 + offset ] = sides ;
		indice[i*3 + offset + 1] = i + sides + 1 ;
		indice[i*3 + offset + 2] = i + sides + 2 ;
	}
}

void cylinder_wire( int sides, GLubyte indice[])
{
	int index;
	for (int i = 0; i < sides ; i++)
	{
		index = i * 2;
		indice[ index ] = i;
		indice[ index + 1 ] = (i + 1) % sides;
	}

	for (int i = 0; i < sides ; i++)
	{
		index = (i + sides) * 2;
		indice[ index ] = i + sides;
		indice[ index + 1 ] = (i + 1) % sides + sides;
	}

	index = sides * 4;
	for (int i = 0; i < sides ; i++)
	{
		indice[ index ] = i;
		indice[ ++index ] = i + sides;
		index ++;
	}
}



