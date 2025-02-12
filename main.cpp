#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include "Button.h"

using namespace sf;
using namespace std;

struct Player {
    string name;
    int score;
};

void create_text(Text& _text, Font& _font, int char_size, int xpos, int ypos);
void add_buttons(vector<Button>& _buttons, vector<string> current_options, Font& _font);
void shuffle_data(vector<vector<string>>* temp_data, vector<vector<string>>* data);
void split_data(vector<vector<string>> &data, vector<string> &questions, vector<int> &correctAnswerIndices, vector<vector<string>> &options);
bool sort_by_score(Player& player_one, Player& player_two);
void sort_players(vector<Player>& _players);
void draw_leaderboard(vector<Player>& _players, vector<Button>& _buttons, Font& _font);
void showInstructions(RenderWindow& app);
void run_app(RenderWindow& app, Font& font, Font& timer_font, fstream& leaderboard);

template<typename T>
auto file_to_data(T& file) {
    vector<vector<string>> data;

    string line;
    while (getline(file, line)) {
        istringstream str(line);
        vector<string> tokens;
        string token;
        while (getline(str, token, '|')) {
            tokens.push_back(token);
        }
        data.push_back(tokens);
    }

    return data;
}

template<typename T>
vector<T> combine_vectors(const vector<T>& vec1, const vector<T>& vec2, const vector<T>& vec3) {
    vector<T> combined_vector;

    // Reserve memory to avoid unnecessary reallocations
    combined_vector.reserve(vec1.size() + vec2.size() + vec3.size());

    // Copy elements from vec1
    for (const auto& elem : vec1) {
        combined_vector.push_back(elem);
    }

    // Copy elements from vec2
    for (const auto& elem : vec2) {
        combined_vector.push_back(elem);
    }

    // Copy elements from vec3
    for (const auto& elem : vec3) {
        combined_vector.push_back(elem);
    }

    return combined_vector;
}


int main() {
    // Set Anti-aliasing options for smooth shapes
    ContextSettings settings;
    settings.antialiasingLevel = 8;

    // Create the main window
    RenderWindow app(VideoMode(1200, 700), "The Via Mzansi Trivia", Style::Titlebar | Style::Close, settings);

    // TEXTURE (Background image)
    Texture texture;
    if (!texture.loadFromFile("0.jpg")) {
        cerr << "Failed to load image file\n";
        return EXIT_FAILURE;
    }
    Sprite sprite;
    sprite.setTexture(texture);

    // Load fonts
    Font font;
    if (!font.loadFromFile("Arial.ttf")){
        cerr << "Failed to load font file \n";
        return EXIT_FAILURE;
    }

    Font timer_font;
    if (!timer_font.loadFromFile("digital-7.ttf")) {
        cerr << "Error loading font" << std::endl;
        return EXIT_FAILURE;
    }

    fstream leaderboard;
    leaderboard.open("leaderboard.txt", fstream::app);

    //Main menu buttons
    Button startButton(525, 550, 150, 50, font, "Start");

    //Instructions button
    Button instructionButton(525, 450, 150, 50, font, "Instructions");

    Text welcome;
    create_text(welcome, font, 50, 230,50);
    welcome.setStyle(Text::Bold);
    welcome.setString("Welcome To The Via Mzansi Trivia");

    while(app.isOpen()){
        Event event;
        while (app.pollEvent(event)){
            if (event.type == Event::Closed){
                app.close();
            }
            if (event.type == Event::MouseButtonPressed){
                if(event.mouseButton.button == Mouse:: Left){
                    Vector2f mousePos = app.mapPixelToCoords(Mouse::getPosition(app));
                    //check if start button is pressed
                    if (startButton.isClicked(mousePos)){
                        run_app(app, font, timer_font, leaderboard);
                    }
                    else if(instructionButton.isClicked(mousePos)){
                        showInstructions(app);
                    }
                }
            }

            if ((event.type == Event::MouseMoved)) {
                Vector2f mousePos = app.mapPixelToCoords(Mouse::getPosition(app));
                Color YELLOW = Color(255, 255, 153);

                // start button highlight
                if (startButton.isHighlighted(mousePos)){
                    startButton.setColor(YELLOW);
                }
                else if(!startButton.isHighlighted(mousePos)){
                    startButton.resetColor();
                }

                // instruction button highlight
                if (instructionButton.isHighlighted(mousePos)){
                    instructionButton.setColor(YELLOW);
                }
                else if(!instructionButton.isHighlighted(mousePos)){
                    instructionButton.resetColor();
                }
            }
        }
        app.clear();
        app.draw(sprite);
        startButton.draw(app);
        instructionButton.draw(app);
        app.draw(welcome);
        app.display();

    }

    return EXIT_SUCCESS;
}


void create_text(Text& _text, Font& _font, int char_size, int xpos, int ypos) {
    _text.setFont(_font);
    _text.setCharacterSize(char_size);
    _text.setPosition(xpos, ypos);
}

void add_buttons(vector<Button>& _buttons, vector<string> current_options, Font& _font) {
    for(int i = 0; i < current_options.size(); ++i){
        if (current_options.size() == 2) {          // 2 buttons (i.e True or False)
            if (i % 2 == 0)
                _buttons.push_back(Button(100, 400 + i * 50, 450, 50, _font, current_options[i]));
            else
                _buttons.push_back(Button(650, 400 + (i-1) * 50, 450, 50, _font, current_options[i]));
        }
        else {                                      // 4 buttons (i.e MCQ)
            if (i % 2 == 0)
                _buttons.push_back(Button(100, 350 + i * 50, 450, 50, _font, current_options[i]));
            else
                _buttons.push_back(Button(650, 350 + (i-1) * 50, 450, 50, _font, current_options[i]));
        }
    }
}

void shuffle_data(vector<vector<string>>* temp_data, vector<vector<string>>* data) {
    int index = 0;
    while (!temp_data->empty()) {
        srand(time(nullptr));
        index = rand() % temp_data->size();

        data->push_back((*temp_data)[index]);
        temp_data->erase(temp_data->begin() + index);
    }
}

void split_data(vector<vector<string>> &data, vector<string> &questions, vector<int> &correctAnswerIndices, vector<vector<string>> &options) {
    // ensure 10 at most is selected (used an array to demonstrate knowledge, could've use a vector)
    array<vector<string>, 10> usable_data;

    for (int i = 0; i < min(10, (int)data.size()); i++) {   // min is for error handling, in case data has < 10 elements
        usable_data[i] = data[i];
    }

    // Extract questions, indices, and options
    for (int i = 0; i < usable_data.size(); i++) {
        if (usable_data[i].empty())
            continue;   // skip empty data

        questions.push_back(usable_data[i][0]);
        correctAnswerIndices.push_back(stoi(usable_data[i][1]));
        usable_data[i].erase(usable_data[i].begin(), usable_data[i].begin() + 2); // erase question and index
        options.push_back(usable_data[i]);  // remaining elements are options
    }

    // Clear data (free memory)
    data.clear();
}

bool sort_by_score(Player& player_one, Player& player_two) {
    return player_one.score > player_two.score;
}

void sort_players(vector<Player>& _players) {
    ifstream _leaderboard("leaderboard.txt");
    _players.clear();   // ensure vector is empty

    string line, token;
    vector<string> tokens;
    vector<vector<string>> temp_players;

    while (getline(_leaderboard, line)) {
        istringstream str(line);
        while (getline(str, token, '|')) {
            tokens.push_back(token);
        }
        temp_players.push_back(tokens);
        tokens.clear();
    }

    for (auto& player : temp_players) {
        _players.push_back(Player{player[0], stoi(player[1])});
    }

    sort(_players.begin(), _players.end(), sort_by_score);
}

void draw_leaderboard(vector<Player>& _players, vector<Button>& _buttons, Font& _font) {
    // fix this so buttons get displayed either way
    _buttons.clear();
    float x_pos;
    for(int i = 0; i < _players.size(); i++){
        if (i == 10)
            break;

        if (i < 5) {
            x_pos = 100;
            _buttons.push_back(Button(x_pos, 150 + i * 70, 400, 50, _font, _players[i].name));
            _buttons.push_back(Button(x_pos, 150 + i * 70, 0, 50, _font, to_string(i+1)));
            _buttons.push_back(Button(x_pos + 450, 150 + i * 70, 0, 50, _font, to_string(_players[i].score)));
        }
        else {
            x_pos = 650;
            _buttons.push_back(Button(x_pos, 150 + (i-5) * 70, 400, 50, _font, _players[i].name));
            _buttons.push_back(Button(x_pos, 150 + (i-5) * 70, 0, 50, _font, to_string(i+1)));
            _buttons.push_back(Button(x_pos + 450, 150 + (i-5) * 70, 0, 50, _font, to_string(_players[i].score)));
        }
    }
}

void showInstructions(RenderWindow& app){
    // TEXTURE (Background image)
    Texture texture;
    if (!texture.loadFromFile("1.jpg")) {
        cerr << "Failed to load image file\n";
    }
    Sprite sprite;
    sprite.setTexture(texture);

    Font font;
    if (!font.loadFromFile("Arial.ttf")){
        cerr << "Failed to load font file \n";
    }

    Text instrHeader;
    create_text(instrHeader, font, 50, 400, 50);
    instrHeader.setStyle(Text::Bold);
    instrHeader.setStyle(Text::Underlined);
    instrHeader.setString("Game Instructions");

    Text inst1;
    create_text(inst1, font, 22, 50, 160);
    inst1.setString("* This is a trivia-styled game that will test the depth of your knowledge about South Africa");

    Text inst2;
    create_text(inst2, font, 22, 50, 210);
    inst2.setString("* The quiz consists of 3 levels which progressively increase in difficulty");

    Text inst3;
    create_text(inst3, font, 22, 50, 260);
    inst3.setString("* You are allowed to abort the game at any point. NOTE: Only the score accumulated will be recorded!");

    Text inst4;
    create_text(inst4, font, 22, 50, 310);
    inst4.setString("* Each question is timed, the time given decreases as the level increases");

    Text inst5;
    create_text(inst5, font, 22, 50, 360);
    inst5.setString("* Once selected, you can not change your choice. Choose wisely!");

    Text inst6;
    create_text(inst6, font, 22, 50, 410);
    inst6.setString("* You can only answer the next question once the question has been answered or once time has elapsed");

    Text inst7;
    create_text(inst7, font, 22, 50, 460);
    inst7.setString("* After you have entered your username press enter to begin the trivia");

    Text inst8;
    create_text(inst8, font, 22, 50, 510);
    inst8.setString("* A leadership board will show the top 10 performers of the game and their score");


    Button menuButton(525, 580, 150, 50, font, "Back to Menu");
    bool instructionRead = false;

    while(app.isOpen() && !instructionRead){
        Event event;
        while (app.pollEvent(event)){
            if (event.type == Event::Closed){
                app.close();
            }
            if (event.type == Event::MouseButtonPressed){
                if(event.mouseButton.button == Mouse:: Left){
                    Vector2f mousePos = app.mapPixelToCoords(Mouse::getPosition(app));
                    //check if menu button is pressed
                    if (menuButton.isClicked(mousePos)){
                        instructionRead = true;
                    }
                }
            }

            if ((event.type == Event::MouseMoved)) {
                Vector2f mousePos = app.mapPixelToCoords(Mouse::getPosition(app));
                Color YELLOW = Color(255, 255, 153);

                if (menuButton.isHighlighted(mousePos)){
                    menuButton.setColor(YELLOW);
                }
                else if(!menuButton.isHighlighted(mousePos)){
                    menuButton.resetColor();
                }
            }
        }
        app.clear();
        app.draw(sprite);
        menuButton.draw(app);
        app.draw(instrHeader);
        app.draw(inst1);
        app.draw(inst2);
        app.draw(inst3);
        app.draw(inst4);
        app.draw(inst5);
        app.draw(inst6);
        app.draw(inst7);
        app.draw(inst8);

        app.display();

    }

}

void run_app(RenderWindow& app, Font& font, Font& timer_font, fstream& leaderboard) {
    Texture texture;
    if (!texture.loadFromFile("2.jpg")) {
        cerr << "Failed to load image file\n";
    }

    Sprite sprite;
    sprite.setTexture(texture);

    // easy
    ifstream easy_file("easy.txt");      // ifstream since we only read from the data file
    auto easy = file_to_data(easy_file);
    easy_file.close();

    vector<vector<string>> easy_data;
    shuffle_data(&easy, &easy_data);

    vector<string> easy_questions;
    vector<int> easy_indices;
    vector<vector<string>> easy_options;
    split_data(easy_data, easy_questions, easy_indices, easy_options);

    // medium
    ifstream medium_file("medium.txt");
    auto medium = file_to_data(medium_file);
    medium_file.close();

    vector<vector<string>> medium_data;
    shuffle_data(&medium, &medium_data);

    vector<string> medium_questions;
    vector<int> medium_indices;
    vector<vector<string>> medium_options;
    split_data(medium_data, medium_questions, medium_indices, medium_options);

    // hard
    ifstream hard_file("hard.txt");
    auto hard = file_to_data(hard_file);
    hard_file.close();

    vector<vector<string>> hard_data;
    shuffle_data(&hard, &hard_data);

    vector<string> hard_questions;
    vector<int> hard_indices;
    vector<vector<string>> hard_options;
    split_data(hard_data, hard_questions, hard_indices, hard_options);

    // combine data

    auto questions = combine_vectors(easy_questions, medium_questions, hard_questions);
    auto correctAnswerIndices = combine_vectors(easy_indices, medium_indices, hard_indices);
    auto options = combine_vectors(easy_options, medium_options, hard_options);

    vector<Button> buttons;
    vector<Button> learderboard_buttons;

    int currentQuestionIndex = 0;

    // add_buttons(buttons, options[currentQuestionIndex], font);
    Button user_name(200, 300, 800, 50, font, "Enter username");
    Button quitButton(1080, 20, 50, 50, font, "Abort");
    Button nextButton(525, 550, 150, 50, font, "Next");
    nextButton.deselect();

    // TEXT OBJECTS
    Text timer;
    create_text(timer, timer_font, 100, 1080, 80);

    Text question;
    create_text(question, font, 30, 50, 70);
    question.setStyle(Text::Bold);
    question.setString("Enter your username to get started!");

    Text message;
    create_text(message, font, 24, 100, 650);
    message.setStyle(Text::Italic);

    Text progress;
    create_text(progress, font, 24, 1000, 650);
    progress.setStyle(Text::Italic);

    Button level(525, 200, 150, 70, font, "Easy");
    level.setColor(Color::Transparent);
    level.setTextColor(Color::White);
    level.setCharSize(50);
    level.centerText();

    Clock clock;
    int init_sec = 15;
    int seconds = -1;        // initialize seconds
    timer.setString(std::to_string(seconds));

    int score = 0;

    // store players
    vector<Player> players;
    string input_text = "";
    string name;

    bool quizCompleted = false;
    bool hasStarted = false;
    bool validUsername = false;
    bool playAgain = false;

	// Start the game loop
    while (app.isOpen() && !playAgain)  {
        // Process events
        Event event;
        while (app.pollEvent(event)) {
            if (event.type == Event::Closed){
                app.close();
            }

            if (quizCompleted) {       // edit this accordingly if quiz is finished
                progress.setString("");
                buttons.clear();
                timer.setString("");
                seconds = -1;

                nextButton.setText("Back to Menu");
                quitButton.setText("Quit");

                // use question message to thank user from playing
                question.setString("Thanks for playing " + name + "! Press 'Back to Menu' to play again.");
                message.setString("Score: " + to_string(score));

                sort_players(players);
                draw_leaderboard(players, learderboard_buttons, font);
            }

            if (currentQuestionIndex >= 19) {
                init_sec = 8;
                if (currentQuestionIndex > 19)
                    level.setText("Hard");
            }
            else if (currentQuestionIndex >= 9) {
                init_sec = 10;
                if (currentQuestionIndex > 9)
                    level.setText("Medium");
            }

            // keyboard events (enter text for user name)
            if (event.type == sf::Event::TextEntered && !hasStarted) {
                if (std::isprint(event.text.unicode))
                    input_text += event.text.unicode;
                    user_name.setText(input_text);
            }
            else if (event.type == sf::Event::KeyPressed && !hasStarted) {
                if (event.key.code == sf::Keyboard::BackSpace) {
                    if (!input_text.empty())
                        input_text.pop_back();
                        user_name.setText(input_text);
                }
                else if (event.key.code == sf::Keyboard::Return && !input_text.empty()) {
                    seconds = init_sec;
                    hasStarted = true;
                    //nextButton.deselect();
                    add_buttons(buttons, options[currentQuestionIndex], font);
                    question.setString(questions[0]);
                    progress.setString("Question " + to_string(currentQuestionIndex + 1) + " of " + to_string(questions.size()));

                    name = input_text;
                    validUsername = true;
                    input_text = "";
                    message.setString("");
                }
            }
            else if (event.type == Event::MouseButtonPressed) {
                if (event.mouseButton.button == Mouse::Left) {
                    Vector2f mousePos = app.mapPixelToCoords(Mouse::getPosition(app));

                    // Check if quitButton is clicked
                    if (quitButton.isClicked(mousePos) && (quitButton.getText() == "Quit")) {
                        app.close();
                    }
                    else if (quitButton.isClicked(mousePos)) {
                        nextButton.select(true);
                        quizCompleted = true;

                        // update leaderboard
                        leaderboard << name << "|" << score << endl;
                        //leaderboard.close();

                        sort_players(players);
                        draw_leaderboard(players, learderboard_buttons, font);
                    }
                    else if (user_name.isClicked(mousePos) && !hasStarted) {
                        if (user_name.getText() == "Enter username")
                            user_name.setText("");
                        message.setString("(Type your username then press the 'Enter Key' to submit)");
                    }

                    if (nextButton.isClicked(mousePos) && nextButton.isSelected() && (nextButton.getText() == "Back to Menu")) {
                        playAgain = true;
                    }
                    else if (nextButton.isClicked(mousePos) && nextButton.isSelected() && (nextButton.getText() == "Next")) {
                        // medium
                        if (currentQuestionIndex == 9) {
                            if (!texture.loadFromFile("3.jpg"))
                                cerr << "Failed to load image file\n";
                            sprite.setTexture(texture);
                        }
                        // hard
                        if (currentQuestionIndex == 19) {
                            if (!texture.loadFromFile("4.jpg"))
                                cerr << "Failed to load image file\n";
                            sprite.setTexture(texture);
                        }
                        seconds = init_sec+1;       // reset seconds for next question

                        // Move to the next question
                        currentQuestionIndex = (currentQuestionIndex + 1) % questions.size();

                        question.setString(questions[currentQuestionIndex]);
                        message.setString("");
                        progress.setString("Question " + to_string(currentQuestionIndex + 1) + " of " + to_string(questions.size()));

                        // disable nextButton
                        nextButton.select(false);
                        nextButton.deselect();

                        // Reset button colors & selection
                        for (auto& button : buttons) {
                            button.resetColor();
                            button.select(false);
                        }

                        buttons.clear();
                        add_buttons(buttons, options[currentQuestionIndex], font);

                        if(currentQuestionIndex == 0){
                            quizCompleted = true;
                            nextButton.resetColor();
                            nextButton.select(true);

                            // update leaderboard
                            leaderboard << name << "|" << score << endl;
                            //leaderboard.close();

                            sort_players(players);
                            draw_leaderboard(players, learderboard_buttons, font);
                        }
                    }

                    // Check if any option button is clicked
                    for (int i = 0; i < buttons.size(); ++i) {
                        if (buttons[i].isClicked(mousePos) && !buttons[i].isSelected() && !nextButton.isSelected()){
                            buttons[i].select(true);
                            nextButton.select(true);

                            // set seconds to an unrealistic value to pause timer
                            seconds = -1;

                            // Check if the clicked button is the correct answer
                            if (i == correctAnswerIndices[currentQuestionIndex]) {
                                score++;
                                buttons[i].isCorrect(true);
                                message.setString("Correct answer!");
                            }
                            else {
                                buttons[i].isCorrect(false);
                                message.setString("Incorrect! The correct answer was: " +
                                                  options[currentQuestionIndex][correctAnswerIndices[currentQuestionIndex]]);
                            }

                            //make the other buttons unselectable
                            for (int j = 0; j < buttons.size(); ++j){
                                if(j != i){
                                    buttons[j].select(true);
                                    buttons[j].deselect();
                                }
                            }
                        }
                    }
                }
            }

            else if ((event.type == Event::MouseMoved)) {
                Vector2f mousePos = app.mapPixelToCoords(Mouse::getPosition(app));
                Color YELLOW = Color(255, 255, 153);

                for (int i = 0; i < buttons.size(); ++i){
                    if (buttons[i].isHighlighted(mousePos) && !buttons[i].isSelected()) {
                        buttons[i].setColor(YELLOW);
                    }
                    else if (!buttons[i].isHighlighted(mousePos) && !buttons[i].isSelected()) {
                        buttons[i].resetColor();
                    }

                    // NEXT
                    if (nextButton.isHighlighted(mousePos) && buttons[i].isSelected()) {
                        nextButton.setColor(YELLOW);
                    }
                    else if(!nextButton.isHighlighted(mousePos) && buttons[i].isSelected()) {
                        nextButton.resetColor();
                    }
                }

                // ABORT
                if (quitButton.isHighlighted(mousePos))
                    quitButton.setColor(YELLOW);
                else if(!quitButton.isHighlighted(mousePos))
                    quitButton.resetColor();

                // this is for Back to Menu button (treating it as next button)
                if (nextButton.isHighlighted(mousePos) && (nextButton.getText() == "Back to Menu"))
                    nextButton.setColor(YELLOW);
                else if (!nextButton.isHighlighted(mousePos) && (nextButton.getText() == "Back to Menu"))
                    nextButton.resetColor();

                if (user_name.isHighlighted(mousePos)&& !validUsername)
                    user_name.setColor(YELLOW);
                else if (!user_name.isHighlighted(mousePos) && !validUsername) {
                    user_name.resetColor();
                    message.setString("");
                }
            }

        }

        // Update timer every second
        if (clock.getElapsedTime().asSeconds() >= 1 && seconds > 0) {
            seconds--;
            timer.setString(std::to_string(seconds));
            clock.restart();
        }
        else if (seconds == 0) {
            // Grey out buttons
            for (int i = 0; i < buttons.size(); i++){
                buttons[i].select(true);
                buttons[i].deselect();
            }

            // Unlock next
            nextButton.select(true);

            message.setString("Oops! Out of Time. The correct answer was: " +
                              options[currentQuestionIndex][correctAnswerIndices[currentQuestionIndex]]);
        }

        // Clear screen
        app.clear();

        app.draw(sprite);
        app.draw(question);
        app.draw(message);
        if (hasStarted) {     // change true
            nextButton.draw(app);
            app.draw(timer);
            app.draw(progress);
            if (!quizCompleted)
                level.draw(app);
            quitButton.draw(app);

            for (auto& button : buttons){       // question/option buttons
                button.draw(app);
            }
        }
        // user_name change true
        if (!validUsername)
            user_name.draw(app);


        for (auto& button : learderboard_buttons){
            button.draw(app);
        }

        // Update the window
        app.display();
    }
}
