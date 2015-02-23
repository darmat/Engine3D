/*************************************************************************
*                                                                        *
*               Computer Graphics - Final Project                        *
*               3D modeling of a combustion engine                       *
*                                                                        *
*  Author:      Matteo Dariol                                            *
*  Date:        12/01/2008                                               *
*                                                                        *
*************************************************************************/

#include <stdio.h>
#include <GL/glut.h>
#include <math.h>

// Defining some constants
#define TRUE  1
#define FALSE 0

#define TEXT_LEN 64
#define TEXT_HEI 64

// Delta angle for each animation frame
#define ANGLE_INCREM 10

// Some geometrical constants
#define AMPLITUDE 120
#define PHASE     270.112
#define FREQ	  58
#define ARC_LEN   2.7
#define ARC_ANGLE 0.15


// Rotation angles
GLdouble view_h = 270, 
		 view_v = 0, 
		 piston_angle = 0;
GLint angle = 0;

// Handle angle increment
GLdouble angle_increm = 5;

// Some toggles
GLshort shadow_tog = TRUE, 
	    anim_tog = FALSE,
		texture_tog = FALSE, 
		trasp_tog = FALSE,
		light_tog = TRUE;

// State of the engine (angle)
GLdouble angular_position[361];

// RGB colours 3D matrix
GLubyte image[TEXT_LEN][TEXT_HEI][3];

// Identificatori per le Display list
GLint shadow_piston_list = 1,
	  texture_piston_list = 2,
	  shadow_flywheel_list = 4,
	  texture_flywheel_list = 8;

// Declaration of GLU object
GLUquadricObj *obj;


// Draw a GLUT cube of dimension 1
void 
draw_cube(GLdouble x, GLdouble y, GLdouble z)
{
	glPushMatrix();
		glScalef(x, y, z);
		if (shadow_tog)
			glutSolidCube(1);
		else
			glutWireCube(1);
	glPopMatrix();
}

// Draw a GLU cylinder with two disk at bottom and top ends
// to simulate a solid object
void 
draw_cylinder(GLUquadricObj * object, GLdouble out_radius, GLdouble in_radius, GLdouble height)
{
	glPushMatrix();
		gluCylinder(object, out_radius, out_radius, height, 20, 1);
		glPushMatrix();
			glRotatef(180, 0.0, 1.0, 0.0);
			gluDisk(object, in_radius, out_radius, 20, 1);
		glPopMatrix();
		glTranslatef(0.0, 0.0, height);
		gluDisk(object, in_radius, out_radius, 20, 1);
	glPopMatrix();
}

// Draw a piston using draw_cylinder function
void 
draw_piston(void)
{
	glPushMatrix();
		glColor4f(0.1, 0.1, 1.0, 1.0);
		glPushMatrix();
			glRotatef(90, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, -0.07);
			draw_cylinder(obj, 0.125, 0.06, 0.12);
		glPopMatrix();
		glRotatef(-90, 1.0, 0.0, 0.0);
		glTranslatef(0.0, 0.0, 0.05);
		draw_cylinder(obj, 0.06, 0.0, 0.6);
		glTranslatef(0.0, 0.0, 0.6);
		draw_cylinder(obj, 0.2, 0.0, 0.5);
	glPopMatrix();
}

// Drawing the engine support
void 
draw_support(void)
{
	glPushMatrix();
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glTranslatef(-0.05, 0.0, 0.0);
		draw_cube(0.5, 3.0, 0.5);
		glColor3f(0.5, 0.1, 0.5);
		glRotatef(90, 0.0, 1.0, 0.0);
		glTranslatef(0.0, 0.9, -0.4);
		draw_cylinder(obj, 0.1, 0.0, 1.5);
	glPopMatrix();
}

// Drawing the head of the piston according to the current angle
void 
draw_cylinder_head(void)
{
	glPushMatrix();
		glColor4f(0.3, 1.0, 0.5, 0.1);
		glRotatef(90, 1.0, 0.0, 0.0);
		glTranslatef(0, 0.0, 0.4);
		glRotatef(piston_angle, 1, 0, 0);
		glTranslatef(0, 0.0, -0.4);
		draw_cylinder(obj, 0.23, 0.21, 1.6);
		glRotatef(180, 1.0, 0.0, 0.0);
		gluDisk(obj, 0, 0.23, 20, 1);
	glPopMatrix();
}

// Draw the flywheel
void 
draw_flywheel(void)
{
	glPushMatrix();
		glColor4f(0.1, 0.1, 1.0, 1.0);
		glTranslatef(0.1, -0.2, 0.0);
		glRotatef(90, 0.0, 1.0, 0.0);
		draw_cylinder(obj, 0.625, 0.0, 0.25);
	glPopMatrix();
}

// This draws the wheel at which the piston is attached to
void 
draw_piston_wheel(void)
{
	glPushMatrix();
		glColor4f(1.0, 1.0, 0.0, 1.0);
		glRotatef(90, 0.0, 1.0, 0.0);
		draw_cylinder(obj, 0.3, 0.08, 0.12);
		glColor4f(0.5, 0.1, 0.5, 1.0);
		glTranslatef(0.0, 0.2, 0.0);
		draw_cylinder(obj, 0.06, 0.0, 0.34);
		glTranslatef(0.0, 0.0, 0.22);
		glRotatef(90, 0.0, 1.0, 0.0);
		glRotatef(angle - piston_angle, 1.0, 0.0, 0.0);
		if (shadow_tog) {
			if (texture_tog)
			glCallList(texture_piston_list);
			else
			glCallList(shadow_piston_list);
		} else
			draw_piston();
	glPopMatrix();
}

// Draws the transmission piston, it must have the same speed as the piston itself
void 
draw_transmission(void)
{
	glPushMatrix();
		glColor3f(0.5, 0.1, 0.5);
		glRotatef(angle, 1.0, 0.0, 0.0);
		glPushMatrix();
			glRotatef(90, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, -0.8);
			draw_cylinder(obj, 0.08, 0.0, 1.2);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.28, 0.0, 0.0);
			draw_piston_wheel();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-0.77, 0.0, 0.0);

			if (shadow_tog) 
			{
				if (texture_tog)
					glCallList(texture_flywheel_list);
				else
					glCallList(shadow_flywheel_list);
			} 
			else
				draw_flywheel();

		glPopMatrix();
	glPopMatrix();
}