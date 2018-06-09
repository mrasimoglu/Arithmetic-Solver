#include <stdio.h>
#include <stdlib.h>

typedef struct VARIABLE
{
	char symbol;
	int val;
	struct VARIABLE *next;
}Variable;

typedef struct STACK
{
	int head;
	int *stack;
	int size;
}Stack;

//Stack Functions
Stack *initializeStack(int);
int pop(Stack *);
int push(Stack *, int);
int isEmpty(Stack *);
int isFull(Stack *);
void printStack(Stack, int);

//Variable Functions
void setVariable(Variable *, char, int);
Variable *initializeVariable();
void addVariable(Variable **, char, int);
void printVariables(Variable *);
int contains(Variable *, char);
int getVariableValue(Variable *, char);

//General Functions
char *infixToPostfix(Stack *, char *);
int solvePostfix(char *, Stack *, Variable *);
void solveFromFile(char *, Stack *, Variable *);

int stringToInt(char *, int *, char);
int stringSize(char *);
void printCurrentValues(char *, int, Stack *);

int main()
{
	Stack *s = initializeStack(100);
	Variable *vars = initializeVariable();
	
	solveFromFile("input.txt", s, vars);
	
	return 0;
}

void solveFromFile(char *filename, Stack *st, Variable *vars)
{
	FILE *file = fopen(filename, "r");
	if(file)
	{
		char *infix = (char*)malloc(sizeof(char));
		char *postfix;
	
		while(!feof(file))
		{
			int i = 0;
			char ch;
			
			ch = fgetc(file);
			while(ch != '\n' && !feof(file))
			{
				infix[i++] = ch;
				infix = (char*)realloc(infix, sizeof(char) * (i + 1));
				ch = fgetc(file);
			}
			infix[i] = '\0';
			
			postfix = infixToPostfix(st, infix);
			solvePostfix(postfix, st, vars);
			printVariables(vars);
			printf("\n\n--------------------------------");
		}
		
		fclose(file);
	}
	else
		printf("File couldn't open.");
}

int getVariableValue(Variable *vars, char sym)
{
	Variable *currentNode = vars;
	
	while(currentNode->next && currentNode->symbol != sym)
		currentNode = currentNode->next;
		
	if(currentNode->symbol == sym)
		return currentNode->val;
	
	return 0;
}

int solvePostfix(char *postfix, Stack *st, Variable *vars)
{
	printf("\n\n--------Solving postfix--------\n\nPostfix : %s", postfix);
	printVariables(vars);
	printf("\n\n");
	
	int val = 0;
	int i = 4;
	int temp, temp2;
	
	while(postfix[i] != ';')
	{
		if(postfix[i] == '!')
		{
			i++;
			push(st, stringToInt(postfix, &i, ' '));
		}
		else if(postfix[i] < 123 && postfix[i] > 96)
		{
			temp = getVariableValue(vars, postfix[i]);
			push(st, temp);
			//printf("\n%c<-%d", postfix[i], temp);
		}
		else if(postfix[i] == '+')
		{
			temp = pop(st);
			temp2 = pop(st);
			push(st, temp + temp2);
		}
		else if(postfix[i] == '-')
		{
			temp = pop(st);
			temp2 = pop(st);
			push(st, temp2 - temp);
		}
		else if(postfix[i] == '*')
		{
			temp = pop(st);
			temp2 = pop(st);
			push(st, temp * temp2);
		}
		else if(postfix[i] == '/')
		{
			temp = pop(st);
			temp2 = pop(st);
			push(st, temp2 / temp);
		}
		
		i = i + 2;
		printf("\n");
		printStack(*st, 1);
	}
	
	val = pop(st);
	
	printf("\n\n%c <- %d", postfix[0], val);
	
	if(contains(vars, postfix[0]))
	{
		setVariable(vars, postfix[0], val);
	}
	else
	{
		addVariable(&vars, postfix[0], val);
	}
}

char *infixToPostfix(Stack *st, char *infix)
{
	printf("\n\n--------Infix to Postfix--------\n\nInfix : %s\n\n", infix);
	
	int i, size = stringSize(infix);
	char *postfix = (char*)malloc(sizeof(char) * size * 2);
	for(i = 0; i < size * 2; i++)
		postfix[i] = ' ';
	for(i = 0; i < 4; i++)
		postfix[i] = infix[i];
	
	int j = 4;
	char temp, temp2;
	
	while(infix[i] != ';')
	{
		if(infix[i] == '(')
			push(st, '(');
		else if(infix[i] == ')')
		{
			temp = pop(st);
			while(temp != '(' && temp != '\0')
			{
				postfix[j] = temp;
				printCurrentValues(postfix, j, st);
				j = j + 2;
				temp = pop(st);
			}
		}
		else if(infix[i] < 58 && infix[i] > 47)
		{
			postfix[j++] = '!';
			while(infix[i] != ' ')
				postfix[j++] = infix[i++];
			j++;
			i--;
		}
		else if(infix[i] < 123 && infix[i] > 96)
		{
			postfix[j] = infix[i];
			j = j + 2;
		}
		else if(infix[i] == '+' || infix[i] == '-' || infix[i] == '*' ||infix[i] == '/')
		{
			temp = pop(st);
			if(temp != '\0')
			{
				temp2 = temp;
				if(temp == '(')
				{
					push(st, temp);
					push(st, infix[i]);
				}
				else if((temp == '+' || temp == '-') && (infix[i] == '*' || infix[i] == '/' || infix[i] == '(' || infix[i] == ')'))
				{
					push(st, temp);
					push(st, infix[i]);
				}
				else if((temp == '*' || temp == '/') && (infix[i] == '(' || infix[i] == ')'))
				{
					push(st, temp);
					push(st, infix[i]);
				}
				else if((infix[i] == '+' || infix[i] == '-') && (temp == '*' || temp == '/' || temp == '(' || temp == ')'))
				{
					postfix[j] = temp;
					temp2 = infix[i];
					j = j + 2;
					do
					{
						temp = pop(st);
						if(temp != '\0')
						{
							postfix[j] = temp;
							j = j + 2;
						}
					} while(temp == '*' || temp == '/' || temp == '(' || temp == ')');
					push(st, temp2);
				}
				else if((infix[i] == '*' || infix[i] == '/') && (temp == '(' || temp == ')'))
				{
					postfix[j] = temp;
					j = j + 2;
					temp = pop(st);
					while(temp == '(' || temp == ')')
					{
						postfix[j] = temp;
						j = j + 2;
						temp = pop(st);
					}
				}
				else if((temp == '*' && infix[i] == '/') || (temp == '/' && infix[i] == '*') || (temp == '+' && infix[i] == '-') || (temp == '-' && infix[i] == '+') || (temp == '-' && infix[i] == '-') || (temp == '+' && infix[i] == '+') || (temp == '*' && infix[i] == '*') || (temp == '/' && infix[i] == '/'))
				{
					postfix[j] = temp;
					j = j + 2;
					push(st, infix[i]);
				}
			}
			else
				push(st, infix[i]);
		}
		i = i + 2;
		printCurrentValues(postfix, j, st);
	}
	
	temp = pop(st);
	while(temp != '\0')
	{
		postfix[j] = temp;
		printCurrentValues(postfix, j, st);
		j = j + 2;
		temp = pop(st);
	}
	
	postfix[j] = ';';
	postfix[j + 1] = '\0';
		
	return postfix;
}

int stringToInt(char *s, int *offset, char token)
{
	int pow = 1;
	int val = 0;
	int i = (*offset);
	
	while(s[*offset + 1] != token)
	{
		pow = pow * 10;
		(*offset)++;
	}
	while(s[i] != token)
	{
		val += (s[i] - 48) * pow;
		pow /= 10;
		i++;
	}
	
	return val;
}

int stringSize(char *s)
{
	int i = 0;
	while(s[i++] != '\0');
	
	return --i;
}

Stack *initializeStack(int size)
{
	Stack* s = (Stack*)malloc(sizeof(Stack));
	s->head = 0;
	s->stack = (int*)malloc(sizeof(int) * size);
	s->size = size;
	return s;
}

int pop(Stack *s)
{
	if(!isEmpty(s))
	{
		s->head--;
		return s->stack[s->head];
	}
	else
		return 0;
	return 0;
}

int push(Stack *s, int c)
{
	if(!isFull(s))
	{
		s->stack[s->head] = c;
		s->head++;
		return 1;
	}
	else
		return 0;
}

int isEmpty(Stack *s)
{
	if(s->head == 0)
		return 1;
	else
		return 0;
}

int isFull(Stack *s)
{
	if(s->head == s->size)
		return 1;
	else
		return 0;
}

void printStack(Stack s, int a)
{
	if(!isEmpty(&s))
	{
		int i;
		printf("Stack: ");
		if(a == 0)
		{
			for(i = 0; i < s.head; i++)
			{
				if(s.stack[i] >= 0)
					printf("%c", s.stack[i]);
			}	
		}
		else
		{
			for(i = 0; i < s.head; i++)
			{
				if(s.stack[i] >= 0)
					printf("%d ", s.stack[i]);
			}
		}
	}
	else
		printf("Stack is empty.");
}

void setVariable(Variable *vars, char sym, int val)
{
	Variable *currentNode = vars;
	
	while(currentNode->next && currentNode->symbol != sym)
		currentNode = currentNode->next;
		
	if(currentNode->symbol == sym)
		currentNode->val = val;
}

Variable *initializeVariable()
{
	Variable *temp = (Variable*)malloc(sizeof(Variable));
	temp->next = NULL;
	
	return temp;
}

void addVariable(Variable **vars, char sym, int val)
{
	Variable *currentNode = *vars;
	
	while(currentNode->next != NULL)
		currentNode = currentNode->next;
	
	currentNode->symbol = sym;
	currentNode->val = val;
	currentNode->next = initializeVariable();
}

void printVariables(Variable *vars)
{
	Variable *currentNode = vars;
	
	printf("\n\nVariables : ");
	if(currentNode->next != NULL)
	{
		while(currentNode->next != NULL)
		{
			printf("%c = %d", currentNode->symbol, currentNode->val);
			currentNode = currentNode->next;
			if(currentNode->next != NULL)
				printf(", ");
		}
	}
	else
		printf("No variable.");
}

int contains(Variable *vars, char ch)
{
	Variable *currentNode = vars;
	
	while(currentNode->next && currentNode->symbol != ch)
		currentNode = currentNode->next;
		
	if(currentNode->symbol == ch)
		return 1;
	
	return 0;
}

void printCurrentValues(char *postfix, int postfixSize, Stack *st)
{
	int i;
	printf("\nPostfix : ");
	for(i = 4; i < postfixSize + 1; i++)
		if(postfix[i] != '!' && postfix[i] != ' ')
			printf("%c", postfix[i]);
	printf("\t\t");
	printStack(*st, 0);
}
