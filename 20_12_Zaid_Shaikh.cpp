#include <iostream>
#include <string>  // for npos and string declarations
#include <fstream> // for file handling
#include <cstring> // for strcpy

// all functions are declared using standard namespace
using namespace std;

// Declaring input and output file classes
ifstream inputFile;
ofstream outputFile;


// Declaring all Functions
void initialize(char*, char*, char*, int, int, int);
void GD(char, int);
void PD(char, int);
void Halt(int&);
void LR(char*, int, char*);
void SR(char*, int, char*);
void CR(char*, int, char*, int);
void BT(int, int&, int&);
void mos(char*, int, int, int);
void loadprogram(char*, char*, char*, int, int, int);
void start_execution(char*, char*, char*, int, int, int);
void execute_user_program(char*, char*, char*, int, int, int);

// Initialize Function
void initialize(char M[100][4], char *IR, char *R, int &IC, int &SI, int &C)
{
    for (int i = 0; i < 100; i++)
        for (int j = 0; j < 4; j++)
            M[i][j] = '\0';
    for (int i = 0; i < 4; i++)
        IR[i] = R[i] = '\0';
    IC = SI = C =  0;

}


// GetData From File Function
void GD(char M[100][4], int mem)
{
    char line[40];
    string line2;
    getline(inputFile, line2);
    strcpy(line, line2.c_str());
    char c = line[0];
    int index = 0;
    int line_byte = 0;

    while (c != '\0')
    {
        M[mem][line_byte] = c;
        index++;
        c = line[index];
        if (line_byte == 3)
            mem += 1;
        line_byte = index % 4;
    }
    //cout << line << endl << endl;  // For printing the data in the Line array
    //cout << line2 << endl << endl; // For printing the data in the string  we read from file
}

// PutData into the File
void PD(char M[100][4], int mem)
{
    char ch, *buffer;
    buffer = (char *)malloc(sizeof(char) * 4);
    int index = 0;
    for (int i = 0; i < 10; i++)
    {
        for (int k = 0; k < 4; k++)
            buffer[k] = M[mem][k];

        for (int j = 0; j < 4; j++)
            outputFile.put(buffer[j]);
        mem += 1;
    }
    outputFile.put('\n');
}

// Halt the program Function
void Halt(int &end_program)
{
    outputFile.put('\n');
    outputFile.put('\n');
    end_program = 1;
}

void LR(char M[100][4], int mem, char *R)
{
    for (int k = 0; k < 4; k++)
        R[k] = M[mem][k];
}

void SR(char M[100][4], int mem, char *R)
{
       for (int k = 0; k < 4; k++)
        M[mem][k] = R[k];
}

void CR(char M[100][4], int mem, char *R, int &C)
{
    char operand2[4];
    int result=1;

    for (int k = 0; k < 4; k++)
    {
        operand2[k] = M[mem][k];
        if(operand2[k]!=R[k])
            {result=0;break;}
    }
    C = result;
}

void BT(int mem, int &IC, int &C)
{
    if (C)
        IC = mem;
    C = 0;
}

void mos(char M[100][4], int mem, int &SI, int &end_program)
{
    switch (SI)
    {
        case 1: GD(M, mem);
                break;
        case 2: PD(M, mem);
                break;
        case 3: Halt(end_program);
                break;
        default:
            cout << "Mos Code is not working !!!" << endl;
    }
    SI = 0;
}

void execute_user_program(char M[100][4], char *IR, char *R, int &IC, int &SI, int &C)
{
    IC = 0, C = 0, SI = 0;
    int end_program = 0;
    int mem,a0,a1;

    while (!end_program)
    {
        for (int k = 0; k < 4; k++)
            IR[k] = M[IC][k];

         a1 = IR[2] - '0';
         a0 = IR[3] - '0';
         mem = a1 * 10 + a0;
        IC++;
        if (IR[0] == 'G')
        {
            SI = 1;
            mos(M, mem, SI, end_program);
        }
        else if (IR[0] == 'P')
        {
            SI = 2;
            mos(M, mem, SI, end_program);
        }
        else if (IR[0] == 'H')
        {
            SI = 3;
            mos(M, mem, SI, end_program);
        }
        else if (IR[0] == 'L')
            LR(M, mem, R);

        else if (IR[0] == 'S')
            SR(M, mem, R);

        else if (IR[0] == 'C')
            CR(M, mem, R, C);

        else if (IR[0] == 'B')
            BT(mem, IC, C);

    }
}

void start_execution(char M[100][4], char *IR, char *R, int &IC, int &SI, int &C)
{
    IC = 0;
    execute_user_program(M, IR, R, IC, SI, C);
}

void loadprogram(char M[100][4], char *IR, char *R, int &IC, int &SI, int &C)
{
    int instruction_count;
    int program_cards;
    int m;
    string line;
    while (!inputFile.eof())
    {
        getline(inputFile, line);
        if (!(line.find("$AMJ") == string::npos))
        {
            m = 0;
            instruction_count = stoi(line.substr(8, 4));
            if (instruction_count % 10 == 0)
                program_cards = instruction_count / 10;
            else
                program_cards = (instruction_count / 10) + 1;

            for (int i = 0; i < program_cards; i++)
            {
                getline(inputFile, line);
                // cout << program_cards << endl;
                for (int j = 0; j < line.length(); j += 4)
                {
                    for (int n = 0; n < 4; n++)
                    {
                        M[m][n] = line[j + n];
                    }
                    m++;
                }
            }
            cout << "\n";
        }
        else if (!(line.find("$DTA") == string::npos))
            start_execution(M, IR, R, IC, SI, C);
        else if (!(line.find("$END") == string::npos))
            initialize(M, IR, R, IC, SI, C);
    }
}

int main()
{
    char M[100][4], IR[4], R[4];
    int C = 0, IC = 0, SI = 0;
    inputFile.open("input_phase1.txt");
    outputFile.open("output_phase1.txt");
    initialize(M, IR, R, IC, SI, C);
    loadprogram(M, IR, R, IC, SI, C);
    inputFile.close();
    outputFile.close();
    return 0;
}



