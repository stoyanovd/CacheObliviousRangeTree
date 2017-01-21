#include <cassert>
#include "TreePrinter.h"

void bst_print_dot_null(int node_repr, int nullcount, FILE *stream)
{
    fprintf(stream, "    null%d [shape=point];\n", nullcount);
    fprintf(stream, "    %d -> null%d;\n", node_repr, nullcount);
}

void bst_print_dot_aux(Node *node, FILE *stream, std::function<int(Node *)> &node_printer)
{
    static int nullcount = 0;

    fprintf(stream, "    %d [label=<%d<BR />"
                " <FONT POINT-SIZE=\"10\">\[%d, %d\]</FONT>"
                "<BR />%d>]\n",
            node_printer(node), node_printer(node),
            node->left_border, node->right_border,
            node->x);

//     a[label=<Birth of George Washington<BR />
//        <FONT POINT-SIZE="10">See also: American Revolution</FONT>>];

    if (node->l)
    {
        assert(node->l->a == node);
        fprintf(stream, "    %d -> %d;\n", node_printer(node), node_printer(node->l));
        bst_print_dot_aux(node->l, stream, node_printer);
    }
    else
    {
        bst_print_dot_null(node_printer(node), nullcount++, stream);
    }

    if (node->r)
    {
        assert(node->r->a == node);
        fprintf(stream, "    %d -> %d;\n", node_printer(node), node_printer(node->r));
        bst_print_dot_aux(node->r, stream, node_printer);
    }
    else
    {
        bst_print_dot_null(node_printer(node), nullcount++, stream);
    }
}

void bst_print_dot(Node *tree, FILE *stream, std::function<int(Node *)> &node_printer)
{
    fprintf(stream, "digraph BST {\n");
    fprintf(stream, "    node [fontname=\"Arial\"];\n");

    if (!tree)
    {
        fprintf(stream, "\n");
    }
    else if (!tree->r && !tree->l)
    {
        fprintf(stream, "    %d;\n", node_printer(tree));
    }
    else
    {
        bst_print_dot_aux(tree, stream, node_printer);
    }

    fprintf(stream, "}\n");
}

void print_tree_to_png(Node *tree, const std::string &filename, std::function<int(Node *)> &&node_printer)
{
    FILE *dot_file = fopen((filename + ".dot").c_str(), "w");
    bst_print_dot(tree, dot_file, node_printer);
    fclose(dot_file);

    std::string dot_command = std::string("dot ") +
        " -Tpng " +
        " -o ../" + filename + ".png" +
        " " + filename + ".dot";
    system(dot_command.c_str());
}