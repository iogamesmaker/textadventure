#include <GL/glut.h>
#include <string>
#include <ctime>
#include <vector>
#include <iostream>
#include <cctype>

bool fullscreen=true,refresh,thing;
int WIDTH = 480, HEIGHT = 360, oWIDTH=480, oHEIGHT=360,level=0,hitpoints=25+level*4,counter=0,gold=0;
float page;
std::string inputText,name, storyText = std::string("\n<b4>Welcome<r0> to another dumb <b4>text adventure game<r0>, or what they call them these days, a <b4>\"visual novel\"<r0>.\n\nPlease type your <b0>name<r0> and press <b0>enter<r0>."), prevInput, prevInputText, lastDialog,where = std::string("<r0>You are in a <b5>tomb<r0>, with people surrounding you doing some kind of <b7>ritual<r0>.\nYou see an <b2><opening><r0> to your left, and <b2><people><r0> to your right.");
int scrollPosition, cursorPosition,lineCount;
void respawn() {
    hitpoints=100;
    page=0;
    lastDialog = std::string("\n\n<b6>You died.\n\n<r0>Why did you do that, you <b6>idiot<r0>...\nWelcome back, mysterious <b7>wizard<r0>, you were out for over a year.\n<b2><What happened?>\n<b2><Who am I?><r0>\n<b2><Where am I?>");
    storyText += lastDialog;
}
struct item {
    std::string name;
    std::string description;
    int price;
    int durability;
    int type; // 1 = weapon 2 = equippable 3 = food 4 = potion 5 = quest item
};

std::vector<item> inventory;

void addItem(const std::string& itemName, const std::string& itemDescription, int itemPrice,int itemType) {
    item newItem;
    newItem.name = itemName;
    newItem.description = itemDescription;
    newItem.price = itemPrice;
    newItem.type = itemType;

    inventory.push_back(newItem);

    storyText += "\n<b2>Added <b0>" + itemName + "<r0> to your inventory.<r0>";
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
            case 4: glColor3f(1.0f, 0.84f,0.0f); break;// Gold
            case 5: glColor3f(0.5f, 0.5f, 0.5f); break;// Gray
            case 6: glColor3f(0.6f, 0.0f, 0.0f); break;// Dark Red
            case 7: glColor3f(0.6f, 0.0f, 0.8f); break;// Purple
            default:glColor3f(1.0f, 1.0f, 1.0f); break;// Default to white
        }
    }
    else{
        switch (index) {
            case 0: glColor3f(0.25f, 0.25f, 0.25f);   break;// White
            case 1: glColor3f(0.2f, 0.0f, 0.0f);      break;// Red
            case 2: glColor3f(0.0f, 0.2f, 0.0f);      break;// Green
            case 3: glColor3f(0.0f, 0.0f, 0.2f);      break;// Blue
            case 4: glColor3f(0.22f, 0.16f, 0.0f);    break;// Gold
            case 5: glColor3f(0.125f, 0.125f, 0.125f);break;// Gray
            case 6: glColor3f(0.15f, 0.0f, 0.0f);     break;// Dark Red
            case 7: glColor3f(0.15f, 0.0f, 0.2f);     break;// Purple
            default:glColor3f(0.25f, 0.25f, 0.25f);   break;// Default to white
        }
    }
}
void renderString(int x, int y, std::string str) {
    glRasterPos2i(x, y);
    for (char c : str) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }
}

void renderText(int x, int y, const char* text) {
    std::string str(text);
    std::string line;
    int colorIndex = 0;
    bool bold=false;
    x-=10;
    int initX=x,initY=y;
    for(int index=0;index<str.length();index++){
        if(x>WIDTH-5){y-=15;x=initX;index--;}
        if(str[index]=='\n'){y-=15;x=initX;
        } else if(str[index]=='<'&&(str[index+1]=='c'||str[index+1]=='b'||str[index+1]=='r')&&str[index+3]=='>'){
            colorIndex = str[index+2]-'0';
            if(str[index+1]=='b'){bold=true;}
            else if(str[index+1]=='r'){bold=false;}
            index+=4;
            if(str[index]=='\n'){y-=15;x=initX;}
        }
        if(y<HEIGHT-30&&y>40){
            if(bold){ // renders text but b o l d
                color(colorIndex,false);
                glRasterPos2i(x-1,y-1);
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[index]);
                color(colorIndex,true);
                glRasterPos2i(x-1,y);
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[index]);
                glRasterPos2i(x,y-1);
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[index]);
                glRasterPos2i(x,y);
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[index]);
            }else {
                color(colorIndex,false);
                glRasterPos2i(x-1,y-1);
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[index]);
                color(colorIndex,true);
                glRasterPos2i(x,y);
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[index]);
            }
        }
        x+=9;
    }
    if (y<25&&refresh){scrollPosition+=1;if(y>=25){refresh=false;}}
}


void renderDoubleString(int x,int y, std::string str,float r,float g,float b,bool bold){
    if(bold){
        glColor3f(r/4,g/4,b/4);
        renderString(x-1,y-1,str);
        glColor3f(r,g,b);
        renderString(x-1,y  ,str);
        renderString(x  ,y-1,str);
        renderString(x  ,y  ,str);
    }else{
        glColor3f(r/4,g/4,b/4);
        renderString(x-1,y-1,str);
        glColor3f(r,g,b);
        renderString(x,y,str);
    }
}
void display() {
    // Background color
    if(hitpoints<10+(level*2)){glClearColor(0.1, 0, 0, 1);} else {glClearColor(0.04, 0.04, 0.1, 1);}
    glClear(GL_COLOR_BUFFER_BIT);
    // Display the story
    renderText(5, (HEIGHT - 23) + scrollPosition * 15, (storyText + "\n").c_str());

    glBegin(GL_QUADS); // Quads of the upper and lower color.
    if(hitpoints<10+(level*2)){ // If low health, make the GUI reddish
    glColor3f(0.05, 0, 0);
    } else {glColor3f(0,0,0.05);}
    glVertex2i(0, HEIGHT);
    glVertex2i(WIDTH, HEIGHT);
    glVertex2i(WIDTH, HEIGHT - 21);
    glVertex2i(0, HEIGHT - 21);
    glVertex2i(0, 0);
    glVertex2i(WIDTH, 0);
    glVertex2i(WIDTH, 47);
    glVertex2i(0, 47);
    glEnd();

    // Display other stats
    renderDoubleString(5, HEIGHT - 15, (inputText).c_str(), 1,1,1,false);
    renderDoubleString(5, 33, (std::to_string(hitpoints) + "/" + std::to_string(25+level*4) + " HP").c_str(), 0.8,0.1,0.1,true);
    renderDoubleString(5, 20, ("LV: " + std::to_string(level)).c_str(),0.3,0.3,1.0,true);
    renderDoubleString(5,7,std::to_string(gold)+" g",1,0.84,0,true);
    // renderDoubleString(WIDTH-95, 5, ("Page " + std::to_string(floorf(page)) + ".").c_str(),0.8,0.8,0.8,true);

    time_t currentTime = time(nullptr); // cursor
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
    bool valid=true;
    std::string input;
    for (char c : in)
        if(c!='.'&&c!='?'&&c!='!'&&c!='/'&&c!='\n'){
        input += std::tolower(c);}
    if(page == 0){name = in;page=1;lastDialog=std::string("\n<b2>Name set to <b0>" + name + "\n\nA <b7>priest<r0> is walking towards you, he is clothed in <b6>dark red<r0>, with artifacts hanging all over him.\n<b4>Welcome back<r0>, our mighty <b7>wizard<r0>. You were lost for over a year.\n<b2><What happened?>\n<b2><Who am I?><r0>\n<b2><Where am I?>");storyText=lastDialog;return;}
    // basic game commands
    if (input == "exit"&&prevInput!="exit") {storyText += std::string("\n<b1>Are you sure you want to <b6>quit<b1>? You will lose all of your progress as a countermeasure. <b2><y/n><r0>");
    } else if(prevInput=="help"&&input=="brackets"){storyText+="\n<c5>that's silly, not what I ment though...\n<r0>";
    } else if (input == "yes" || input == "y" || input == "yes."){if      (prevInput == "exit") {exit(0);}else if (prevInput == "respawn") {respawn();}else {storyText+= "\n<b1>Command not found. Enter <c2><what><b1> to view the last story-related dialog, or <b2><h><r0> <b1>for help.<r0>";valid=false;}
    } else if (input == "no"  || input == "n") {if(prevInput == "exit" || prevInput == "respawn") {storyText += "\n<b2>Cancelled action.<r0>";prevInput = "";} else {storyText+= "\n<b1>Command not found. Enter <c2><what><b1> to view the last story-related dialog, or <b2><h><r0> <b1>for help.\n<r0>";valid=false;}
    } else if (input == "respawn"&&prevInput!="respawn") {prevInput=std::string("respawn");storyText += std::string("\n<b1>Are you sure you want to <c6>respawn<c1>? You will lose all of your progress. <b2><y/n><r0>");
    } else if (input == "clear") {storyText = "\n<b2>Cleared the screen.<r0>" + lastDialog;inputText.clear();cursorPosition = 0;scrollPosition = 0;
    } else if (input == "help" || input == "h" || input == "?") {input = "help";storyText += "\n<b2><H><r0>elp menu\n<b2><what><r0> - shows the last story-related dialog.\n<b2><exit><r0> - quits the game.\n<b2><clear><r0> - clears the screen.\n<b2><respawn><r0> - restart the game.\nEnter stuff that's between <b2><brackets><r0> to play the game.";
    } else if (input == "what"){storyText += "\n" + lastDialog;
    } else if (input == "where am i" || input == "where") {storyText+="\n" + where;
    // Page 1
    } else if (input == "what happened" || input == "what" && page==1){storyText+= "\n<r0>We don't, exactly know, <b7>wizard<r0>, you just disappeared for a while until one of our miner crew found this tomb. Are you doing OK?";
    } else if (input == "who am i"||input == "who" && page==1) {storyText+="\n<r0>You are <b0>" + name + "<r0>, our saviour! Apparently you're some kind of <b7>wizard<r0>. You feel <c5>weak<c0>.";
    } else if (input == "opening" && (page==1||page==100001)) {page = 2;where = "You are in a blooming little village, people are everywhere. You see mountains from afar.\n";storyText += lastDialog = std::string("\n<r0>You <b0>exit<r0> the cave.\nYou find a beautiful village, containing a <b2><market><r0>, and a <b2><house for sale><r0>.\nThe streets are crowded with many different species, a wild deer even felt so safe,\nthat it walks among the crowd.\nYou can also go back to the <b2><cave><r0>");
    } else if (input == "people" && page==1){storyText+="\nYou ask a person what they're doing.\nThey respond with a faint <b5>Oh...<r0>, and run away. You see <c1>candles<c0> on the floor.";
    // Page 100001 (back into the cave after exiting.)
    } else if (input == "inscription" && page==100001){storyText+="\nThe <b5>inscription<r0> on the <b5>tomb<r0> states that the person lying within is a <b0>mighty<b5> wizard<r0> that saved the village.\nYou don't understand what's mighty about you. Mighty weak, you think.\nYou see the <b2><opening><r0>.";
    // Page 2
    } else if (input == "cave" && page == 2) {page = 100001;where="You are in an empty cave, with a <b5>tomb<r0> in it. Light is leaking in from the <b2>opening<r0>.";storyText+=lastDialog="\nThe cave is empty; everyone left. You see an <b2><inscription><r0> on the <b5>tomb<r0>.";
    } else if (input == "market" && page == 2) {page = 3;where="You are at a <b0>market<r0>. People are all over the place, there is not a single empty spot to be found.\nThere are people selling fish, fruit, weapons, armor, all kinds of things you need <b4>Gold<r0> for.";
    // Other commands
    } else if (input.find("get") != std::string::npos || input.find("pick up") != std::string::npos) {// pickup/drop logic here
    } else if (input == "respawn" || input == "exit") { storyText += "\n<b1>You just did that.<b2> <y/n><r0>";
    } else {storyText+= "\n<b1>Command not found. Enter <c2><what><b1> to view the last story-related dialog, or <b2><h><b1> for help.<r0>";valid=false;}
    if(valid){prevInput = input;}
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
        storyText += "\n<r5>" + inputText + "<c0>";
        prevInputText = inputText;
        inputText.clear();
        cursorPosition = 0;
        handleCommand(prevInputText);
    }
    refresh=true;
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: if(page==0){exit(0);}else{if(prevInput!="quit"){storyText+=std::string("\n<b1>Are you sure you want to <c6>exit<c1>? You will lose all of your progress as a countermeasure. <b2><y/n><r0>");prevInput=std::string("exit");break;}}
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
        case GLUT_KEY_LEFT:  if (cursorPosition > 0) cursorPosition--; break;
        case GLUT_KEY_RIGHT: if (cursorPosition < inputText.length()) cursorPosition++; break;
        case GLUT_KEY_DOWN:  refresh=false;scrollPosition += 1; break;
        case GLUT_KEY_UP:    refresh=false;scrollPosition -= 1; if (scrollPosition == -1) scrollPosition = 0; break;
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
    glutCreateWindow("Text adventure Alpha v0.0.1");

    glClearColor(0.04, 0.04, 0.1, 1);
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glutFullScreen();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}
