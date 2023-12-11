#include <GL/glut.h>
#include <string>
#include <ctime>
#include <iostream>
#include <cctype>

bool fullscreen,refresh;
int WIDTH = 800, HEIGHT = 600, oWIDTH, oHEIGHT,page,hitpoints=100;
std::string inputText, storyText = std::string("<c0>\nWelcome to yet another dumb adventure game.\nEnter <c2><h><c0> for help. Input stuff that's between brackets to play the game.\n\nYou are in a room dark room, hanging upside down.\nDo you attempt to <c2><leave><c0> or <c2><kill yourself><c0>.\n"), prevInput, prevInputText, lastDialog=storyText;
int scrollPosition, cursorPosition,lineCount;
void respawn(){
    hitpoints=100;
    page=0;
    lastDialog = std::string("\n\n<c6>You died.<c0>\nWhy did you do that, you idiot...\n\nYou are in a room, hanging upside down. Do you attempt to <c2><leave> <c0>or <c2><kill yourself><c0>.\n");
    storyText += lastDialog;
}
inline int floorf(float x) {
    return static_cast<int>(x) - static_cast<int>(x < 0 && x != static_cast<int>(x));
}
void color(int index, bool light) {
    if(light){
        switch (index) {
            case 0: glColor3f(1.0f, 1.0f, 1.0f); break;// White
            case 1: glColor3f(0.8f, 0.0f, 0.0f); break;// Red
            case 2: glColor3f(0.0f, 0.8f, 0.0f); break;// Green
            case 3: glColor3f(0.0f, 0.0f, 0.8f); break;// Blue
            case 4: glColor3f(1.0f, 0.84f, 0.0f);break;// Gold
            case 5: glColor3f(0.5f, 0.5f, 0.5f); break;// Gray
            case 6: glColor3f(0.6f, 0.0f, 0.0f); break;// Dark Red
            default:glColor3f(1.0f, 1.0f, 1.0f); break;// Default to white
        }
    }
    else{
        switch (index) {
            case 0: glColor3f(0.25f, 0.25f, 0.25f);   break;// White
            case 1: glColor3f(0.2f, 0.0f, 0.0f);      break;// Red <c1> green <c2> blue <c3> gold <c4> gray <c5> dark red <c6> white <c0>
            case 2: glColor3f(0.0f, 0.2f, 0.0f);      break;// Green
            case 3: glColor3f(0.0f, 0.0f, 0.2f);      break;// Blue
            case 4: glColor3f(0.25f, 0.22f, 0.0f);    break;// Gold
            case 5: glColor3f(0.125f, 0.125f, 0.125f);break;// Gray
            case 6: glColor3f(0.2f, 0.0f, 0.0f);      break;// Dark Red
            default:glColor3f(0.25f, 0.25f, 0.25f);   break;// Default to white
        }
    }
}
void renderText(int x, int y, void* font, const char* text, bool input) {
    const char* originalText = text;
    glRasterPos2i(x - 1, y - 1);
    int x0 = x, y0 = y;
    int lineWidth = 0, lineCount = 0;
    while (*text) {
        if (*text == '\n') {
            y -= 15;
            glRasterPos2i(x - 1, y - 1);
            lineWidth = 0;
            lineCount++;
        } else if (*text == '<' && *(text + 1) == 'c' && isdigit(*(text + 2)) && *(text + 3) == '>') {
            color(*(text + 2),false);
            text += 4;
            continue;
        } else {
            glutBitmapCharacter(font, *text);
            lineWidth += 9;
        }
        if (!input && lineWidth > WIDTH - 10) {
            y -= 15;
            glRasterPos2i(x - 1, y - 1);
            lineWidth = 0;
            lineCount++;
        }
        ++text;
    }

    x = x0;
    y = y0;
    text = originalText;
    glRasterPos2i(x, y);
    lineWidth = 0, lineCount = 0;
    while (*text) {
        if (*text == '\n') {
            y -= 15;
            glRasterPos2i(x, y);
            lineWidth = 0;
            lineCount++;
        } else if (*text == '<' && *(text + 1) == 'c' && isdigit(*(text + 2)) && *(text + 3) == '>') {
            color(*(text + 2),true);
            text += 4;
            continue;
        } else {
            glutBitmapCharacter(font, *text);
            lineWidth += 9;
        }
        if (!input && lineWidth > WIDTH - 10) {
            y -= 15;
            glRasterPos2i(x, y);
            lineWidth = 0;
            lineCount++;
        }
        ++text;
    }
    if (scrollPosition < lineCount - (floorf(HEIGHT / 15) - 5) && refresh && !input) {
        scrollPosition = lineCount - (floorf(HEIGHT / 15) - 5);refresh=false; }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    renderText(5, (HEIGHT - 20) + scrollPosition * 15, GLUT_BITMAP_9_BY_15, storyText.c_str(), false);


    glBegin(GL_QUADS);
    glColor3f(0, 0, 0.05);
    glVertex2i(0, HEIGHT);
    glVertex2i(WIDTH, HEIGHT);
    glVertex2i(WIDTH, HEIGHT - 21);
    glVertex2i(0, HEIGHT - 21);
    glVertex2i(0, 0);
    glVertex2i(WIDTH, 0);
    glVertex2i(WIDTH, 47);
    glVertex2i(0, 47);
    glEnd();

    renderText(5, 33, GLUT_BITMAP_9_BY_15, ("<c6>" + std::to_string(hitpoints) + " HP").c_str(), true);
    renderText(WIDTH-95, 5, GLUT_BITMAP_9_BY_15, ("<c0>Page " + std::to_string(page) + ".").c_str(), true);
    renderText(5, HEIGHT - 15, GLUT_BITMAP_9_BY_15, ("<c0>" + inputText).c_str(), true);

    time_t currentTime = time(nullptr); // cursor (Blinks)
    if((currentTime % 2 == 0)) {
        glBegin(GL_LINES);
        glColor3f(1, 1, 1);
        glVertex2i(6 + cursorPosition * 9, HEIGHT - 1);
        glVertex2i(6 + cursorPosition * 9, HEIGHT - 16);
        glColor3f(0.25, 0.25, 0.25);
        glVertex2i(7 + cursorPosition * 9, HEIGHT - 1);
        glVertex2i(7 + cursorPosition * 9, HEIGHT - 16);
        glEnd();
    }

    glBegin(GL_LINES);
    glColor3f(0.25,0.25,0.25);
    glVertex2i(0, HEIGHT - 22);
    glVertex2i(WIDTH, HEIGHT - 22);
    glVertex2i(0, 47);
    glVertex2i(WIDTH, 47);
    glEnd();

    glutSwapBuffers();
}

void handleCommand(const std::string& in) {
    std::string input;
    for (char c : in)
        input += std::tolower(c);
    if (input == "exit") {
        storyText += std::string("<c6>\nAre you sure you want to exit? You will lose all of your progress. <c3><y/n><c0>");
    } else if (input == "yes" || input == "y"){
        if      (prevInput == "exit") {exit(0);}
        else if (prevInput == "respawn") {respawn();}
        else {storyText+= "<c0>\nCommand not found. Enter <c3><what><c0> to view the last story-related dialog.\n<c0>";}
    } else if (input == "no"  || input == "n") {
        if      (prevInput == "exit" || prevInput == "respawn") {storyText += "Cancelled action.";} else {storyText+= "<c0>\nCommand not found. Enter <c3><what><c0> to view the last story-related dialog.\n<c0>";}
    } else if (input == "respawn") {
        storyText += std::string("<c6>\nAre you sure you want to respawn? You will lose all of your progress. <c3><y/n><c0>");
    } else if (input == "clear") {
        storyText = "<c0>\nCleared the screen." + lastDialog;
        inputText.clear();
        cursorPosition = 0;
        scrollPosition = 0;
    } else if (input == "help" || input == "h" || input == "?") {
        storyText += "\nWelcome to my little adventure game. Here are the commands I added so far.\n<c3><inv><c0> - see your inventory\n<c3><drop (item)><c0> - drops one of your items.\n<c3><use (item) (person/creature)><c0> - uses an item, to heal or attack someone.\n<c3><clear><c0> - clears the screen.\n<c3><exit><c0> - exit the game.\n<c3><what><c0> - show the last story-related dialog.";
    } else if (input == "what"){storyText += "\n" + lastDialog;}

    else {storyText+= "<c0>\nCommand not found. Enter <c3><what><c0> to view the last story-related dialog.\n<c0>";}
    refresh=true;
    prevInput = input;
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    WIDTH = w;
    HEIGHT = h;
}

void handleEnter() {
    if (inputText.length() > 0) {
        storyText += "\n" + inputText;
        prevInputText = inputText;
        inputText.clear();
        cursorPosition = 0;
        handleCommand(prevInputText);
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: exit(0);
        case 13: handleEnter(); break;
        case 8:
            if (!inputText.empty() && cursorPosition > 0) {
                inputText.erase(cursorPosition - 1, 1);
                cursorPosition--;
            }
            break;
        case 127:
            if (cursorPosition < inputText.length() + 1)
                inputText.erase(cursorPosition, 1);
            break;
        default:
            inputText.insert(cursorPosition, 1, key);
            cursorPosition++;
            break;
    }
}

void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT: if (cursorPosition > 0) cursorPosition--; break;
        case GLUT_KEY_RIGHT: if (cursorPosition < inputText.length()) cursorPosition++; break;
        case GLUT_KEY_DOWN: scrollPosition += 1; break;
        case GLUT_KEY_UP: scrollPosition -= 1; if (scrollPosition == -1) scrollPosition = 0; break;
        case GLUT_KEY_F11:
            fullscreen = !fullscreen;
            if (fullscreen) {
                oWIDTH = WIDTH;
                oHEIGHT = HEIGHT;
                glutFullScreen();
            } else {
                glutReshapeWindow(oWIDTH, oHEIGHT);
            }
            break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Text Adventure - Alpha 0.01");

    glClearColor(0.04, 0.04, 0.1, 1);
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}
