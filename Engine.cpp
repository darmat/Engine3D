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
#include "Engine.h"

// This function draws the entire model.
// First it cleans the rendering buffer, then verifies the transparency
// toggle in order to identify which part to actually draw.
void 
display(void)
{
	int pass;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

		if (trasp_tog) 
		{
			glEnable(GL_ALPHA_TEST);
			pass = 2;
		} 
		else 
		{
			glDisable(GL_ALPHA_TEST);
			pass = 0;
		}

		glRotatef(view_h, 0, 1, 0);
		glRotatef(view_v, 1, 0, 0);

		do {
			if (pass == 2) 
			{
				glAlphaFunc(GL_EQUAL, 1);
				glDepthMask(GL_TRUE);
				pass--;
			} 
			else if (pass != 0) 
			{
				glAlphaFunc(GL_NOTEQUAL, 1);
				glDepthMask(GL_FALSE);
				pass--;
			}

			draw_support();
				glPushMatrix();
				glTranslatef(0.5, 1.4, 0.0);
				disegna_testa_cilindro();
				glPopMatrix();
				glPushMatrix();
				glTranslatef(0.0, -0.8, 0.0);
				draw_transmission();
			glPopMatrix();
		} while (pass > 0);

		glDepthMask(GL_TRUE);
		glutSwapBuffers();
	glPopMatrix();
}

// This is the animation routine, called when the window is idle.
// It updates the angle of rotation.
void 
animation(void)
{
	if ((angle += angle_increm) >= 360)
		angle = 0;

	piston_angle = angular_position[angle];
	glutPostRedisplay();
}

// This is a callback activated every time a key is pressed
// it identifies which command to execute
void 
keyboard(unsigned char key, int x, int y)
{
	switch (key) 
	{
		// Toggle Shadows
		case 's':
		case 'S':
			if (shadow_tog == FALSE) 
			{
				shadow_tog = TRUE;
				glShadeModel(GL_SMOOTH);
				glEnable(GL_LIGHTING);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_COLOR_MATERIAL);
				gluQuadricNormals(obj, GLU_SMOOTH);
				gluQuadricDrawStyle(obj, GLU_FILL);
			} 
			else 
			{
				shadow_tog = FALSE;
				glShadeModel(GL_FLAT);
				glDisable(GL_LIGHTING);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_COLOR_MATERIAL);
				gluQuadricNormals(obj, GLU_NONE);
				gluQuadricDrawStyle(obj, GLU_LINE);
				gluQuadricTexture(obj, GL_FALSE);
			}

			if (!texture_tog && shadow_tog) break;

		// Toggle Textures
		case 'd':
		case 'D':
			if (texture_tog == FALSE) 
			{
				texture_tog = TRUE;
				glEnable(GL_TEXTURE_2D);
				gluQuadricTexture(obj, GL_TRUE);
			} 
			else 
			{
				texture_tog = FALSE;
				glDisable(GL_TEXTURE_2D);
				gluQuadricTexture(obj, GL_FALSE);
			}
			break;

		// Toggle Transparency
		case 'f':
		case 'F':
			if (trasp_tog == FALSE) 
			{
				trasp_tog = TRUE;
			} 
			else 
			{
				trasp_tog = FALSE;
			}
			break;

		// Increase rotation angle
		case 'a':
		case 'A':
			if ((angle += angle_increm) >= 360)
				angle = 0;
			piston_angle = angular_position[angle];
			break;

		// Decrease rotation angle
		case 'z':
		case 'Z':
			if ((angle -= angle_increm) <= 0)
				angle = 360;
			piston_angle = angular_position[angle];
			break;

		// Toggle Light
		case 'x':
		case 'X':
			if (light_tog) 
			{
				glDisable(GL_LIGHT0);
				light_tog = FALSE;
			} else 
			{
				glEnable(GL_LIGHT0);
				light_tog = TRUE;
			}
			break;

		// Rotate view (TILT +)
		case '4':
			if ((view_h -= ANGLE_INCREM) <= 0)
				view_h = 360;
		break;

		// Rotate view (TILT -)
		case '6':
			if ((view_h += ANGLE_INCREM) >= 360)
				view_h = 0;
		break;

		// Rotate view (PAN +)
		case '8':
			if ((view_v += ANGLE_INCREM) >= 360)
				view_v = 0;
		break;

		// Rotate view (PAN -)
		case '2':
			if ((view_v -= ANGLE_INCREM) <= 0)
				view_v = 360;
		break;

		// Toggle Animation
		case ' ':
			if (anim_tog) 
			{
				glutIdleFunc(0);
				anim_tog = FALSE;
			} 
			else 
			{
				glutIdleFunc(animation);
				anim_tog = TRUE;
			}
			break;

		// Increase Engine rotation
		case '+':
			if ((++angle_increm) > 45)
				angle_increm = 45;
			break;

		// Decrease Engine rotation
		case '-':
			if ((--angle_increm) <= 0)
				angle_increm = 0;
			break;

		// Quit 
		case 'q':
		case 'Q':
			exit(0);
			break;

		default:
			return;
	}

	glutPostRedisplay();
}

// This function changes the observer's point of view
void
change_POV(int key, int x, int y)
{
	switch (key) 
	{
		case GLUT_KEY_LEFT:
			if ((view_h -= ANGLE_INCREM) <= 0)
				view_h = 360;
			break;

		case GLUT_KEY_RIGHT:
			if ((view_h += ANGLE_INCREM) >= 360)
				view_h = 0;
			break;

		case GLUT_KEY_UP:
			if ((view_v += ANGLE_INCREM) >= 360)
				view_v = 0;
			break;

		case GLUT_KEY_DOWN:
			if ((view_v -= ANGLE_INCREM) <= 0)
				view_v = 360;
			break;

		default:
			return;
	}
	glutPostRedisplay();
}

// This function maps the menu options to the "keyboard" function
// toggles and options
void 
menu(int val)
{
	unsigned char key;

	switch (val) 
	{
		case 1:
			key = 's';
			break;

		case 2:
			key = ' ';
			break;

		case 3:
			key = 'd';
			break;

		case 4:
			key = 'f';
			break;

		case 5:
			key = 'x';
			break;

		case 6:
			key = '+';
			break;

		case 7:
			key = '-';
			break;

		case 8:
			key = 'q';
			break;

		default:
			return;
	}

	keyboard(key, 0, 0);
}

// This function generates a simple checker texture
void
genTexture(void)
{
	int i, j, c;

	for (i = 0; i < TEXT_LEN; i++) 
	{
		for (j = 0; j < TEXT_HEI; j++) 
		{
			c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
			image[i][j][0] = (GLubyte) c;
			image[i][j][1] = (GLubyte) c;
			image[i][j][2] = (GLubyte) c;
		}
	}
}

// This calculates all the possible positions (states) for
// the engine moving parts
void 
genPositions(void)
{
	GLint i;
	GLdouble k;

	for (i = 0, k = 0.0; i < 360; i++, k++) 
	{
		angular_position[i] = AMPLITUDE * atan((ARC_ANGLE * sin(PHASE - k / FREQ)) /
											   ((ARC_LEN - ARC_ANGLE * cos(PHASE - k / FREQ))));
	}
}

// Initialize the scene
void 
init(void)
{
	GLfloat posizione_luce[] = {2.0, 2.0, 2.0, 0.0};
	glClearColor(0.0, 0.0, 0.0, 0.0);
	obj = gluNewQuadric();

	genPositions();
	genTexture();

	// Fix the texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, TEXT_LEN, TEXT_HEI, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Fix the lights
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, posizione_luce);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
  
	// Fix all the display lists
	glNewList(shadow_piston_list, GL_COMPILE);
	draw_piston();
	glEndList();
	glNewList(shadow_flywheel_list, GL_COMPILE);
	draw_flywheel();
	glEndList();

	gluQuadricTexture(obj, GL_TRUE);
	glNewList(texture_piston_list, GL_COMPILE);
	draw_piston();
	glEndList();
	glNewList(texture_flywheel_list, GL_COMPILE);
	draw_flywheel();
	glEndList();
	gluQuadricTexture(obj, GL_FALSE);
}

// This function is called every time the main window is modified
void 
reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.0, (GLfloat) w / (GLfloat) h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);
	glScalef(1.5, 1.5, 1.5);
}

// Main function
int 
main(int argc, char **argv)
{
	puts(" Animation Commands: \n");
	puts(" POV			   : arrows");
	puts(" Engine Rotation : 'a' = CCW        'z' = CW");
	puts(" Rotation Speed  : '+' = increase   '-' = decrease");
	puts(" Toggles         : 's' = shadows    'd' = texture");
	puts("                   ' ' = animate    'f' = transparency");
	puts("                   'x' = light");
	puts(" Quit            : 'q' \n");
	puts(" Mouse click to display menu \n");

	glutInitWindowSize(500, 600);
	glutInitWindowPosition(300, 100);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutCreateWindow(" 3D Engine Animation ");

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(change_POV);

	glutCreateMenu(menu);
	glutAttachMenu(GLUT_LEFT_BUTTON);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAddMenuEntry("Shadows", 1);
	glutAddMenuEntry("Animate", 2);
	glutAddMenuEntry("Texture", 3);
	glutAddMenuEntry("Transparency", 4);
	glutAddMenuEntry("Light", 5);
	glutAddMenuEntry("Add Speed", 6);
	glutAddMenuEntry("Reduce Speed", 7);
	glutAddMenuEntry("Quit", 8);

	init();
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;            
}
