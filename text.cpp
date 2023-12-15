// Welcome to the terrible backend of my stuff
// #define FREEGLUT_STATIC
#define GLEW_STATIC // static, so you all don't have to install GLEW yourself :) (unless you are modding the source code and compiling it)
#include <GL/glew.h>
#include <GL/freeglut.h> // Freeglut also works w/ static linking, but only on Linux. You windows people will need an extra DLL in the same directory as the program.
#include <string>
#include <ctime>
#include <vector>
#include <iostream>
#include <cctype>

bool fullscreen=true,refresh,thing=false,house=false; // a mess.
int WIDTH = 460, HEIGHT = 360, oWIDTH=460, oHEIGHT=360,level=0,hitpoints=25,counter=0,gold=0,mana=0,xp=5;
float page;
std::string inputText,name, storyText = std::string("\n<b4>Welcome<r0> to another dumb <b4>text adventure game<r0>, or what some call them, a <b4>\"visual novel\"<r0>.\n\nPlease type your <b0>name<r0> and press <b0>enter<r0>."), prevInput, prevInputText, lastDialog,where = std::string("<r0>You are in a <b5>tomb<r0>, with people surrounding you doing some kind of <b7>ritual<r0>.\nYou see an <b2><opening><r0> to your left, and <b2><people><r0> to your right.");
int scrollPosition, cursorPosition,lineCount;
int random(int a,int b){return rand() % a + b;}
void respawn() {
    hitpoints=100;
    page=0;
    storyText = lastDialog = std::string("\n<b6>You died...\nRest in piece,<b0> "+name+"<r0>.\n<b4>Welcome back<r0> to another dumb <b4>text adventure game<r0>, or what they call them these days, a <b4>\"visual novel\"<r0>.\n\nPlease type your <b0>name<r0> and press <b0>enter<r0>.");
}
struct monster {


};
int spellCooldown[6] = {0,0,0,0,0,0};
struct spell {
    std::string name;
    int atk;
    int spread; // damage = (atk*(level/20))*(1-(spread-random(0,spread*2)));
    int mana;   // mana-= water.spell(cost);
    int level;
    int cooldown;
};

std::vector<spell> spells;
void addSpell(const std::string& spellName, int atk, int spread, int mana, int level, int cooldown) {
    spell newSpell;
    newSpell.name = spellName;
    newSpell.atk = atk;
    newSpell.spread = spread;
    newSpell.mana = mana;
    newSpell.level = level;
    newSpell.cooldown = cooldown;
    spells.push_back(newSpell);
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
        if(y<HEIGHT-30&&y>62){
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
    if (y<55&&refresh){scrollPosition+=1;if(y>=55){refresh=false;}}
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
    if(hitpoints<10+(level*2)){glClearColor(0.1, 0, 0, 1);} else {glClearColor(0.02, 0.02, 0.02, 1);}
    glClear(GL_COLOR_BUFFER_BIT);
    // Display the story
    renderText(5, (HEIGHT - 23) + scrollPosition * 15, (storyText + "\n").c_str());

    glBegin(GL_QUADS); // Quads of the upper and lower color.
    if(hitpoints<10+(level*2)){ // If low health, make the GUI reddish
    glColor3f(0.05, 0, 0);
    } else {glColor3f(0.05,0.05,0.05);}
    glVertex2i(0, HEIGHT);
    glVertex2i(WIDTH, HEIGHT);
    glVertex2i(WIDTH, HEIGHT - 21);
    glVertex2i(0, HEIGHT - 21);
    glVertex2i(450, 0);
    glVertex2i(WIDTH, 0);
    glVertex2i(WIDTH, 62);
    glVertex2i(450, 62);
    if(hitpoints<10+(level*2)){ // If low health, make the GUI reddish
    glColor3f(0.1, 0, 0);
    } else {glColor3f(0.05,0.05,0.05);}
    glVertex2i(0,0);
    glVertex2i(0,62);
    glVertex2i(90,62);
    glVertex2i(90,0);
    glColor3f(0.1,1,0.1);
    glVertex2i((xp/(20+level*0.1))*360+90,0);
    glVertex2i((xp/(20+level*0.1))*360+90,15);
    glVertex2i(90,15);
    glVertex2i(90,0);
    glColor3f(0.02,0.02,0.02);
    glEnd();

    // Display other stats
    renderDoubleString(92, 3, (std::to_string(xp) + "/" + std::to_string(floorf(20+level*0.1))+"XP").c_str(), 0.05,0.05,0.05,true);
    renderDoubleString(5, HEIGHT-15, (inputText).c_str(), 1,1,1,false);
    renderDoubleString(5, 48, (std::to_string(hitpoints) + "/" + std::to_string(25+level*4) + " HP").c_str(), 0.8,0,0,true);
    renderDoubleString(5, 34, (std::to_string(mana) + "/" + std::to_string(level*3) + " MA").c_str(),0,0,0.8,true);
    renderDoubleString(5, 19, (std::to_string(level)+" LV").c_str(),0.1,0.25,1.0,true);
    renderDoubleString(5, 5,std::to_string(gold)+" G",1,0.84,0,true);
    // Display spell stats (AAA)

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
    glVertex2i(0, 61);
    glVertex2i(WIDTH, 61);
    glVertex2i(90,61);
    glVertex2i(90,0 );
    // Spell GUI
    glVertex2i(150,61);
    glVertex2i(150,15);

    glVertex2i(210,61);
    glVertex2i(210,15);

    glVertex2i(270,61);
    glVertex2i(270,15);

    glVertex2i(330,61);
    glVertex2i(330,15);

    glVertex2i(390,61);
    glVertex2i(390,15);

    glVertex2i(450,61);
    glVertex2i(450,0);

    //
    glVertex2i(90,15);
    glVertex2i(450,15);

    glEnd();

    glutSwapBuffers();
    if(refresh){glutPostRedisplay();}
}

// Actual game code starts here

void handleCommand(const std::string& in) { // The messiest function of them all, just a shit ton of possibilities. don't know how to organise them otherwise.
    bool valid=true;
    std::string input;
    for (char c : in)
        if(c!='.'&&c!='?'&&c!='!'&&c!='/'&&c!='\n'){
        input += std::tolower(c);}
    if(page == 0){name = in;page=1;lastDialog=std::string("\n<b2>Name set to <b0>" + name + "<r0>\n\nA <b7>priest<r0> is walking towards you, he is clothed in <b6>dark red<r0>, with artifacts hanging all over him.\n<b4>Welcome back<r0>, our mighty <b7>wizard<r0>. You were lost for over a year.\n<b2><What happened?>\n<b2><Who am I?><r0>\n<b2><Where am I?>");storyText=lastDialog;return;}
    // basic game commands
    if (input == "exit"&&prevInput!="exit") {storyText += std::string("\n<b1>Are you sure you want to <b6>quit<b1>? You will lose all of your progress as a countermeasure. <b2><y/n><r0>");
    } else if(prevInput=="help"&&input=="brackets"){storyText+="\n<c5>that's silly, not what I ment though...\n<r0>";
    } else if (input == "respawn"&&prevInput!="respawn") {prevInput=std::string("respawn");storyText += std::string("\n<b1>Are you sure you want to <c6>respawn<c1>? You will lose all of your progress. <b2><y/n><r0>");
    } else if (input == "clear") {storyText = "\n<b2>Cleared the screen.<r0>" + lastDialog;inputText.clear();cursorPosition = 0;scrollPosition = 0;
    } else if (input == "help" || input == "h" || input == "?") {input = "help";storyText += "\n<b2><H><r0>elp menu\n<b2><what><r0> - shows the last story-related dialog.\n<b2><exit><r0> - quits the game.\n<b2><clear><r0> - clears the screen.\n<b2><respawn><r0> - restart the game.\n<b2><where><r0> - Gives a summary of your surroundings\nEnter stuff that's between <b2><brackets><r0> to play the game.";
    } else if (input == "what"){storyText += "\n" + lastDialog;
    } else if (input == "where am i" || input == "where") {storyText+="\n" + where;
    // Page 1 (cave)
    } else if (input == "what happened" || input == "what" && page==1){storyText+= "\n<r0>We don't, exactly know, <b7>wizard<r0>, you just disappeared for a while until one of our miner crew found this tomb. Are you doing OK?";
    } else if (input == "who am i"||input == "who" && page==1) {storyText+="\n<r0>You are <b0>" + name + "<r0>, our saviour! Apparently you're some kind of <b7>wizard<r0>. You feel <c5>weak<c0>.";
    } else if (((input == "leave"||input=="leave the conversation"||input=="leave conversation") && page == 4)||(input == "village" && page == 3)||(input == "opening" && (page==1||page==100001))) {page = 2;where = "You are in a blooming little village, people are everywhere. You see mountains from afar.\n";if(house){storyText+=lastDialog=std::string("\nYou find a beautiful village next to the <b5>mysterious <b2><cave><r0>. The enchanting village contains a <b2><market><r0> near the center,\nand, of course your <b2>home<r0>\nThe streets are crowded with many different species, a wild deer even felt so safe that it walks among the crowd.\nThe background contains a large <b3>mystical forest<r0>, with huge mushrooms between the trees.");}else{storyText += lastDialog = std::string("\nYou find a beautiful village next to the <b5>mysterious <b2><cave><r0>. The enchanting village contains a <b2><market><r0> near the center,\nand a <b2><house for sale><r0> near the residential area.\nThe streets are crowded with many different species, a wild deer even felt so safe that it walks among the crowd.\nThe background contains a large <b3>mystical forest<r0>, with huge mushrooms between the trees.");}
    } else if (input == "people" && page==1){storyText+="\nYou ask a person what they're doing.\nThey respond with a faint <b5>Oh...<r0>, and run away. You see <c1>candles<c0> on the floor.";
    } else if ((input == "yes"||input == "im ok"||input == "ok"||input=="y"||input == "yeah"||input == "im doing fine"||input == "im good"||input == "all good")&&page==1&&prevInput=="what happened"){storyText+="\nThank god you are doing OK!";
    } else if ((input == "no"||input=="terrible"||input == "n"||input=="bad"||input == "nah"||input == "not at all"||input == "not good")&&page==1&&prevInput=="what happened"){storyText+="\n<c5>The <b7>priest<r5> is whispering something to the others";
    // Page 100001 (back into the cave after exiting.)
    } else if (input == "inscriptions" && page==100001){storyText+="\nThe <b5>inscription<r0> on the <b5>tomb<r0> states that the person lying within is a <b0>mighty<b7> wizard<r0> that saved the village.\nYou don't understand what's mighty about you. Mighty weak, you think.\nYou see the <b2><opening><r0>.";
    // Page 2 (village 1)
    } else if (input == "cave" && page == 2) {page = 100001;where="\nYou are in an empty cave, with a <b5>tomb<r0> in it. Light is leaking in from the <b2>opening<r0>.";storyText+=lastDialog="\nThe cave is empty; everyone left. You notice <b2><inscriptions><r0> on the <b5>tomb<r0>.";
    } else if (input == "market" && page == 2) {page = 3;storyText+=lastDialog="\nYou go to the <b4>market<r0>.\nYou see many little shops selling various goods for <b4>gold<r0>.\n<b2><shops>\n<village><r0>";where="You are at a <b0>market<r0>. People are all over the place, there is not a single empty spot to be found.\nThere are people selling fish, fruit, weapons, armor, all kinds of things you need <b4>gold<r0> for. You see <b5>mountains<r0> in the distance, the weather is clear. The nice sunlight is penetrating trough the <b0>clouds<r0> like it's fogged glass.";
    } else if ((input == "house"||input == "house for sale")&&page == 2){page = 4;where = "\nYou are at a nice, cozy house that's for sale. The current owner is happily working in the little garden, waiting for someone to buy the place.";storyText+=lastDialog="\nYou walk towards the <b2>house for sale<r0>, making your way trough the crowd.\nPeople nicely step aside for you to cross, smiling as you walk by them.\nWhen you made it across, the house owner approaches you, saying hi and asking if you are interested.\n<b2><Hi there!>\n<How much does it cost?>\n<Buy house>\n<Leave the conversation>";
    // Page 3 (market)
    // stuff to be added
    // Page 4 (house)
    } else if ((input == "hi"||input == "hi there"||input=="hello there"||input=="hello")&&page==4){storyText+="\nYou say <b0>" +in+ "<r0> to the house owner.\nHe responds with <b0>\"Hi, <c2>" + name + "!<c0> I'm glad you took a look at my home for sale. Are you interested in buying it? It would be an honor.\"<r0>";if(gold<24){storyText+="\n<c5>You look in your empty purse, and sigh.<r0>";}
    } else if ((input == "cost"||input =="how much does it cost"||input=="price"||input == "how much")&&page==4){storyText+="\nYou ask the house owner how much the property cost.\nHe states that it costs <b4>300 gold<r0>.";if(gold>270&&gold<300){storyText+="\nYou bargain for a little while, and managed to bring the price down to <b4>270 gold<r0>, if you buy it now.\n";thing=true;valid=false;}
    } else if ((input == "buy"||input == "buy house"||input=="purchase")&&page==4) {page = 5;storyText+="This piece of property costs <b4> ";if(thing){storyText+="270";}else{storyText+="300";}storyText+=" gold<r0>. Do you want to buy it? <b2><y/n><c0>";
    } else if ((input == "no"||input == "n")&&page==5){storyText+="\n<b6>What are you wasting my time for, then?";page=4;
    } else if ((input == "y"||input == "yes")&&page==5){storyText+="\nHouse purchased.";if(thing){gold-=270;}else{gold-=300;}
    // Other commands
    // } else if (input.find("get") != std::string::npos || input.find("pick up") != std::string::npos) {// pickup/drop logic here
    } else if (input == "respawn" || input == "exit") { storyText += "\n<b1>You just did that.<b2> <y/n><r0>";
    } else if (input == "yes" || input == "y" || input == "yes."){if      (prevInput == "exit") {exit(0);}else if (prevInput == "respawn") {respawn();}else {storyText+= "\n<b1>Command not found. Enter <c2><what><b1> to view the last story-related dialog, or <b2><h><r0> <b1>for help.<r0>";valid=false;}
    } else if (input == "no"  || input == "n") {if(prevInput == "exit" || prevInput == "respawn") {storyText += "\n<b2>Cancelled action.<r0>";prevInput = "";} else {storyText+= "\n<b1>Command not found. Enter <c2><what><b1> to view the last story-related dialog, or <b2><h><r0> <b1>for help.\n<r0>";valid=false;}

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
    glutPostRedisplay();
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
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: if(page==0){exit(0);}else{if(prevInput!="quit"){inputText.clear();cursorPosition=0;storyText+=std::string("\n<b1>Are you sure you want to <c6>exit<c1>? You will lose all of your progress as a countermeasure. <b2><y/n><r0>");prevInput=std::string("exit");glutPostRedisplay();break;}}
        case 13: handleEnter(); break;
        case 8:
            if (!inputText.empty() && cursorPosition > 0) {
                inputText.erase(cursorPosition - 1, 1);
                cursorPosition--;
                glutPostRedisplay();
            }
            break;
        case 127:
            if (cursorPosition < inputText.length() + 1){
                inputText.erase(cursorPosition, 1);glutPostRedisplay();
            }
            break;
        default:
            inputText.insert(cursorPosition, 1, key);
            cursorPosition++;
            glutPostRedisplay();
            break;
    }
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:  if (cursorPosition > 0){ cursorPosition--;}glutPostRedisplay();break;
        case GLUT_KEY_RIGHT: if (cursorPosition < inputText.length()){ cursorPosition++;glutPostRedisplay; }break;
        case GLUT_KEY_DOWN:  refresh=false;scrollPosition += 1;glutPostRedisplay(); break;
        case GLUT_KEY_UP:    refresh=false;scrollPosition -= 1; if (scrollPosition == -1) {scrollPosition = 0;glutPostRedisplay();} break;
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
void timer(int state)
{
    glutPostRedisplay();
    glutTimerFunc(999, timer, 0);
}
int main(int argc, char** argv) {
    addSpell("Meelee",   10, 7 ,0, 0, 0); // Spell name, attack, spread, mana, level, cooldown
    addSpell("Shitstorm",12, 5 ,1, 2, 0); // Temporary spells
    addSpell("Water",    20, 20,2, 6, 0);
    addSpell("Attack",   40, 10,4, 12,0);
    addSpell("Epic",     100,30,10,17,0);
    addSpell("Magic",    250,25,25,25,0);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Text Adventure - Alpha v0.0.2");

    glewInit();

    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glutFullScreen();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0,timer,0);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    glutPostRedisplay();
    glutMainLoop();
    return 0;
}
