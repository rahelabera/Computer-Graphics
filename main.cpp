#include <GL/glut.h>
#include <math.h>
#include <sstream>

// Ball properties
float ballRadius = 0.05f;
float ballX = 0.5f;
float ballY = 0.5f;
float ballSpeedX;         // Speed in the X direction
float ballSpeedY;         // Speed in the Y direction
float ballSpeed = 0.01f;  // Total speed magnitude
float ballAngle = 20.0f;  // Initial angle in degrees

int score = 0;

// Box properties
float boxWidth = 0.2f;
float boxHeight = 0.1f;
float boxX = 0.4f;
float boxSpeed = 0.05f;

// Window size
int windowWidth = 800;
int windowHeight = 600;

// Game state
bool gameOver = false;

// Initialize OpenGL
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gluOrtho2D(0.0, 1.0, 0.0, 1.0);

    // Convert initial angle to radians
    float angleInRadians = ballAngle * (3.141592 / 180.0);

    // Set the initial speed components based on the angle
    ballSpeedX = ballSpeed * cos(angleInRadians);
    ballSpeedY = ballSpeed * sin(angleInRadians);
}

// Function to draw the ball
void drawBall() {
    glColor3f(0.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < 360; i++) {
        float theta = i * 3.141592 / 180;
        float x = ballX + ballRadius * cos(theta);
        float y = ballY + ballRadius * sin(theta);
        glVertex2f(x, y);
    }
    glEnd();
}

// Function to draw the box
void drawBox() {
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(boxX, 0.0f);
    glVertex2f(boxX + boxWidth, 0.0f);
    glVertex2f(boxX + boxWidth, boxHeight);
    glVertex2f(boxX, boxHeight);
    glEnd();
}

// Function to draw text on the screen
void drawText(float x, float y, std::string text) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);

    for (size_t i = 0; i < text.length(); ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void updateScore() {
    score = score+5;
}

// Function to reset the game state
void resetGame() {
    ballX = 0.5f;
    ballY = 0.5f;
    ballAngle = 30.0f;
    float angleInRadians = ballAngle * (3.141592 / 180.0);
    ballSpeedX = ballSpeed * cos(angleInRadians);
    ballSpeedY = ballSpeed * sin(angleInRadians);
    boxX = 0.4f;
    gameOver = false;
    score = 0;
}

// Function to handle ball movement and collisions
void update(int value) {
    if (!gameOver) {
        // Update ball position
        ballX += ballSpeedX;
        ballY += ballSpeedY;

        // Check for collisions with window boundaries
        if (ballX + ballRadius > 1.0f || ballX - ballRadius < 0.0f) {
            // Reverse the x direction on collision
            ballSpeedX = -ballSpeedX;
        }
        if (ballY + ballRadius > 1.0f || ballY - ballRadius < 0.0f) {
            // Reverse the y direction on collision
            ballSpeedY = -ballSpeedY;
        }

        // Check for collision with the box
        if (ballX + ballRadius > boxX && ballX - ballRadius < boxX + boxWidth &&
            ballY + ballRadius > 0.0f && ballY - ballRadius < boxHeight) {
            // Reverse the ball's direction on collision with the box
            ballSpeedX = -ballSpeedX;
            ballSpeedY = -ballSpeedY;
            updateScore();  // Increment the score on box collision
        }

        // Game over condition: Ball collides with the bottom window
        if (ballY - ballRadius < 0.0f) {
            gameOver = true;
        }

        // Update box position with user input
        glutPostRedisplay();

        // Set the next timer
        glutTimerFunc(16, update, 0);
    }
}

// Function to handle drawing the scene
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the ball
    drawBall();

    // Draw the box
    drawBox();

    // Display "Game Over" message when the game is over
    if (gameOver) {
        drawText(0.4f, 0.5f, "Game Over! Press 'R' to replay.");
    }

    // Display score at the top
    std::stringstream scoreText;
    scoreText << "Score: " << score;
    drawText(0.02f, 0.95f, scoreText.str());

    glutSwapBuffers();
}

// Function to handle arrow key input for the box movement
void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            boxX -= boxSpeed;
            break;
        case GLUT_KEY_RIGHT:
            boxX += boxSpeed;
            break;
    }

    // Ensure the box stays within the window boundaries
    if (boxX < 0.0f) boxX = 0.0f;
    if (boxX + boxWidth > 1.0f) boxX = 1.0f - boxWidth;

    // Redraw the scene
    glutPostRedisplay();
}

// Function to handle keyboard input
void keyboard(unsigned char key, int x, int y) {
    float angleInRadians;
    switch (key) {
        case 'R':
        case 'r':
            if (gameOver) {
                resetGame();
                gameOver = false;
                glutTimerFunc(16, update, 0);  // Start the game loop again
            }
            break;
        case 'A':
        case 'a':
            // Change ball's angle by 10 degrees when 'A' key is pressed
            ballAngle += 10.0f;
            angleInRadians = ballAngle * (3.141592 / 180.0);
            ballSpeedX = ballSpeed * cos(angleInRadians);
            ballSpeedY = ballSpeed * sin(angleInRadians);
            break;
        case 'S':
        case 's':
            // Increase or decrease box speed when 'S' key is pressed
            boxSpeed += 0.01f;
            break;
        case 'D':
        case 'd':
            // Decrease box speed when 'D' key is pressed (if speed is greater than 0)
            if (boxSpeed > 0.01f) {
                boxSpeed -= 0.005f;
            }
            break;
        case 'Q':
        case 'q':
        case 27: // ESC key
            exit(0);
            break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Ball and Box Collision");

    init();

    // Set the function callbacks
    glutDisplayFunc(display);
    glutTimerFunc(16, update, 0);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    return 0;
}
