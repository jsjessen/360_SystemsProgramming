#include "tree.h"
#include "parse.h"

static Node* root; // root of the file tree
static Node* cwd; // current working directory


int initialize()
{
	root = NULL;
	root = add_node(NULL, "/", 'D');

	if(!root) 
		return -1;

	cwd = root;

	return 0;
}

Node* add_node(char* dirname, char* basename, char type)
{
	Node* newNode = NULL;
	Node* parent = NULL;
	Node* sibling = NULL;

	if(root)
	{
		parent = get_dir(dirname);

		if(!parent || !isDir(parent) || get_node(dirname, basename))
			return NULL;
	}

	// Use of calloc means node properties default to null.
	newNode = (Node*)calloc(1, sizeof(Node));
	if(newNode == NULL)
		return NULL;

	newNode->name = (char*)malloc((strlen(basename) + 1));
	if(newNode->name == NULL)
		return NULL;

	strcpy(newNode->name, basename);
	newNode->type = type;

	if(!parent)
		return newNode; // root

	newNode->parent = parent;

	if(parent->child)
	{
		sibling = get_last_child(parent);
		sibling->sibling = newNode;
	}
	else
		parent->child = newNode;

	return newNode;
}

int remove_node(char* dirname, char* basename, char type)
{
	Node* deleting = get_node(dirname, basename);

	if(type == 'F' && !isFile(deleting))
	{
		fprintf(stderr, "Not a file\n");
		return -1;
	}
	else if(type == 'D' && !isDir(deleting))
	{
		fprintf(stderr, "Not a directory\n");
		return -1;
	}

    if(remove_node2(deleting) != 0)
        return -1;

    return 0;
}

int remove_node2(Node* deleting)
{
	Node* sibling = NULL;

	if(!deleting || hasChild(deleting))
        return -1;

    if(deleting != root)
    {
        sibling = get_sibling(deleting);

        if(sibling) // Older sibling point to younger
            sibling->sibling = deleting->sibling; 
        else		// Parent point to next child
            deleting->parent->child = deleting->sibling; 
    }

    printf("Removing Node: %s\n", deleting->name);

    free(deleting->name);
    free(deleting);

    return 0;
}

int set_cwd(char* dirname, char* basename)
{
    Node* tmp = NULL;

    if(!dirname && !basename)
    {
        cwd = root;
        return 0;
    }

    tmp = get_node(dirname, basename);
    if(!tmp || isFile(tmp))
        return -1;

    cwd = tmp;
    return 0;
}

Node* get_dir(char* dirname)
{
    Node* current;
    char *target;

    if(!dirname)
        return cwd;

    if(isAbsolute(dirname))
        current = root; // absolute
    else 		
        current = cwd; // relative

    char* path = (char*)malloc(strlen(dirname) + 1);
    strcpy(path, dirname);

    target = strtok(path, "/"); // break up into tokens
    while(target)
    {
        current = get_child(target, current);
        if (!current)
        {
            free(path);
            return NULL; // No child of current matches target
        }

        target = strtok(0, "/"); // continue to tokenize path
    }

    free(path);
    return current;
}

Node* get_node(char* dirname, char* basename)
{
    Node* dir = get_dir(dirname);
    if(!dir || !isDir(dir) || !hasChild(dir))
        return NULL;

    return get_child(basename, dir);
}

Node* get_sibling(Node* child)
{
    Node* sibling = child->parent->child;

    while(sibling->sibling)
    {
        if(sibling->sibling == child)
            return sibling;

        sibling = sibling->sibling;
    }

    return NULL;
}

Node* get_last_child(Node* parent)
{
    Node* sibling = parent->child;

    while(sibling->sibling)
        sibling = sibling->sibling;

    return sibling;
}

Node* get_child(char* childname, Node* parent)
{
    Node* child = NULL;

    if(!parent || !isDir(parent) || !hasChild(parent))
        return NULL; 

    child = parent->child;

    while (child && strcmp(childname, child->name) != 0) 
        child = child->sibling;

    return child;
}

bool hasChild(Node* node)
{
    if(node->child)
        return true;
    else
        return false;
}

bool isDir(Node* node)
{
    if(node->type == 'D')
        return true;
    else
        return false;
}

bool isFile(Node* node)
{
    if(node->type == 'F')
        return true;
    else
        return false;
}

bool isAbsolute(char* path)
{
    if(!path)
        return true;

    if(path[0] == '/')
        return true;
    else
        return false;
}

bool isRelative(char* path)
{
    if(path[0] == '/')
        return false;
    else
        return true;
}

int print_cwd(char** wd)
{
    return rpwd(cwd, wd);
}

int rpwd(Node* dir, char** wd)
{
    if(dir == root)
    {
        strcat(*wd, "/");
        return 0;
    }

    if(rpwd(dir->parent, wd) != 0)
        return -1;

    strcat(*wd, dir->name);
    strcat(*wd, "/");

    return 0;
}

int print_children(char* dirname, char* basename)
{
    Node* nPtr = NULL;

    if(basename)
    {
        nPtr = get_node(dirname, basename);
        if(!nPtr)
            return -1;
    }
    else
        nPtr = cwd;

    if (isFile(nPtr))
    {
        printf("%c \t %s\n", nPtr->type, nPtr->name);
        return 0;
    }

    // List all children
    nPtr = nPtr->child;
    while(nPtr)
    {
        printf("%c \t %s\n", nPtr->type, nPtr->name);
        nPtr = nPtr->sibling;  // Next child
    }

    return 0;
}

int print_tree(FILE *out)
{
    return rec_print_tree(root, out);
}

int rec_print_tree(Node* current, FILE *out)
{
    char* wd = NULL;

    if (current == NULL)
        return 0;

    wd = (char*)calloc(256, 1);

    if(rpwd(current, &wd) != 0)
    {
        free(wd);
        return -1;
    }

    fprintf(out, "%c %s\n", current->type, wd); // Current 
    rec_print_tree(current->child, out);
    rec_print_tree(current->sibling, out);

    free(wd);
    return 0;
}

int delete_tree()
{
    printf("Warning: Deleting Tree!\n");
    return rec_delete_tree(root);
}

int rec_delete_tree(Node* current)
{
    if(!current)
        return 0;

    if(rec_delete_tree(current->child) != 0)
        return -1;

    if(rec_delete_tree(current->sibling) != 0)
        return -1;

    return remove_node2(current);
}

int build_tree(FILE *save_file)
{
    char buf[256];
    char* type = NULL;
    char* pathname = NULL;
    char* dirname = NULL;
    char* basename = NULL;

    bool firstline = true;

    if(delete_tree() != 0)
        return -1;

    if(initialize() != 0)
        return -1;

    // get and print line string
    while(fgets(buf, 256, save_file) != NULL)
    {
        if(firstline)
        {
            firstline = false;
            continue;
        }

        free(type);
        free(pathname);
        free(dirname);
        free(basename);

        // get rid of newline
        buf[strlen(buf)- 1] = 0;

        if(parse_input(buf, &type, &pathname) != 0)
        {
            fprintf(stderr, "Failed to parse the line\n");
            continue;
        }
        else
            printf("Command = \"%s\"\nPathname = \"%s\"\n", type, pathname);
        if(parse_path(pathname, &dirname, &basename) != 0)
        {
            fprintf(stderr, "Failed to parse the path\n");
            continue;
        }
        else
            printf("dirname = \"%s\"\nbasename = \"%s\"\n", dirname, basename);

        add_node(dirname, basename, *type);
    }

    if(feof(save_file))
        return 0;
    else
        return -1;
}
