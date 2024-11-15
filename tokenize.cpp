#include "headers.h"

vector<string> tokenize(string input)
{
    char *token;
    char *inputString = const_cast<char *>(input.c_str());
    vector<string> tokens;
    bool insideQuotes = false;

    while ((token = strtok(inputString, " \t")) != nullptr)
    {
        if (insideQuotes)
        {
            tokens.back() += " " + string(token);
            if (token[strlen(token) - 1] == '"')
            {
                insideQuotes = false;
            }
        }
        else if (token[0] == '"')
        {
            insideQuotes = true;
            tokens.push_back(string(token));
            if (token[strlen(token) - 1] == '"')
            {
                insideQuotes = false;
            }
        }
        else
        {
            tokens.push_back(string(token));
        }
        inputString = nullptr;
    }
    return tokens;
}

bool checkIfIODelimiter(char c)
{
    return (c == '<' || c == '>');
}

bool checkIfPipeDelimiter(char c)
{
    return c == '|';
}

vector<string> tokenizeForRedirection(string input)
{
    vector<string> tokens;
    string currentToken;
    bool insideQuotes = false;

    for (int i = 0; i < input.size(); ++i)
    {
        char c = input[i];

        if (c == '"')
        {
            insideQuotes = !insideQuotes;
            currentToken += c;
        }
        else if (!insideQuotes && checkIfIODelimiter(c))
        {
            if (!currentToken.empty())
            {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            while (i + 1 < input.size() && (input[i + 1] == ' ' || input[i + 1] == '\t'))
            {
                i++;
            }
            if (c == '>' && i < input.size() - 1 && input[i + 1] == '>')
            {
                tokens.push_back(">>");
                ++i;
            }
            else
            {
                tokens.push_back(std::string(1, c));
            }
        }
        else
        {
            currentToken += c;
        }
    }

    if (!currentToken.empty())
    {
        tokens.push_back(currentToken);
    }

    return tokens;
}

vector<string> tokenizeForPipe(string input)
{
    vector<string> tokens;
    string currentToken;
    bool insideQuotes = false;

    for (int i = 0; i < input.size(); ++i)
    {
        char c = input[i];

        if (c == '"')
        {
            insideQuotes = !insideQuotes;
            currentToken += c;
        }
        else if (!insideQuotes && checkIfPipeDelimiter(c))
        {
            if (!currentToken.empty())
            {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            while (i + 1 < input.size() && (input[i + 1] == ' ' || input[i + 1] == '\t'))
            {
                i++;
            }

            tokens.push_back(std::string(1, c));
        }

        else
        {
            currentToken += c;
        }
    }

    if (!currentToken.empty())
    {
        tokens.push_back(currentToken);
    }

    return tokens;
}

vector<string> tokenizeForColon(string input)
{
    vector<string> tokens;
    string currentToken;
    bool insideQuotes = false;

    for (int i = 0; i < input.size(); ++i)
    {
        char c = input[i];

        if (c == '"')
        {
            insideQuotes = !insideQuotes;
            currentToken += c;
        }
        else if (!insideQuotes && c == ';')
        {
            if (!currentToken.empty())
            {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            while (i + 1 < input.size() && (input[i + 1] == ' ' || input[i + 1] == '\t'))
            {
                i++;
            }

            tokens.push_back(std::string(1, c));
        }

        else
        {
            currentToken += c;
        }
    }

    if (!currentToken.empty())
    {
        tokens.push_back(currentToken);
    }

    return tokens;
}
