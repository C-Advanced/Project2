#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dllist.h"
#include "jrb.h"
#define INFINITIVE 10000000
#define END -9999
#define POS_SIZE 60

typedef struct
{
    JRB edges;
    JRB vertices;
} Graph;

typedef struct
{
    char line[10][10];
    int n_line;
    char k[POS_SIZE];
    char p[POS_SIZE];
    double d;
    int t;
} vtx;

typedef struct
{
    char line[10][10];
    int n_line;
    int begin;
    int end;
} Guide;

Graph createGraph()
{
    Graph graph;
    graph.edges = make_jrb();
    graph.vertices = make_jrb();
    return graph;
}

void addVertex(Graph graph, char *name)
{
    JRB find = jrb_find_str(graph.vertices, name);
    if (find == NULL)
        jrb_insert_str(graph.vertices, name, new_jval_i(1));
    return;
}

// char *getVertex(Graph graph, int id)
// {
//     JRB find;
//     find = jrb_find_str(graph.vertices, id);
//     if (find == NULL)
//         return NULL;
//     else
//         return jval_s(find->val);
// }

// void addEdge(Graph g, char* pos1, char* pos2, double weight)
// {
//     JRB Node = jrb_find_str(g.edges, pos1);
//     JRB tree;
//     if (Node == NULL)
//     {
//         tree = make_jrb();
//         jrb_insert_str(g.edges, pos1, new_jval_v(tree));
//     }
//     else
//     {
//         tree = (JRB)jval_v(Node->val);
//     }
//     jrb_insert_str(tree, pos2, new_jval_d(weight));
// }

int strcmp_Ncasesen(char *s1, char *s2)
{
    int len1 = strlen(s1), len2 = strlen(s2), i;
    if (len1 != len2)
        return 1;
    for (i = 0; i < len1; ++i)
        if (tolower(s1[i]) != tolower(s2[i]))
            return 1;
    return 0;
}

void addEdge(Graph g, char *pos1, char *pos2, char *lineval)
{
    JRB ptr;
    JRB Node = jrb_find_str(g.edges, pos1);
    JRB tree, line;
    int i = 0;
    if (Node == NULL)
    {
        tree = make_jrb();
        line = make_jrb();
        jrb_insert_str(line, strdup(lineval), new_jval_i(1));
        jrb_insert_str(tree, strdup(pos2), new_jval_v(line));
        jrb_insert_str(g.edges, strdup(pos1), new_jval_v(tree));
        return;
    }
    else
    {
        tree = (JRB)jval_v(Node->val);
    }
    // jrb_insert_str(tree, pos2, new_jval_d(weight));
    Node = jrb_find_str(tree, pos2);
    if (Node == NULL)
    {
        line = make_jrb();
        jrb_insert_str(tree, strdup(pos2), new_jval_v(line));
    }
    else
        line = jval_v(Node->val);
    jrb_insert_str(line, strdup(lineval), new_jval_i(1));
}

int hasEdge(Graph graph, char *pos1, char *pos2)
{
    JRB find = jrb_find_str(graph.edges, pos1);
    if (find == NULL)
        return 0;
    if (jrb_find_str(jval_v(find->val), pos2) != NULL)
        return 1;
    return 0;
}

double getEdgeValue(Graph graph, char *pos1, char *pos2)
{
    JRB find = jrb_find_str(graph.edges, pos1), edge;
    if (find == NULL)
        return INFINITIVE;
    edge = jrb_find_str(jval_v(find->val), pos2);
    if (edge == NULL)
        return INFINITIVE;
    return jval_d(edge->val);
}

int outdegree(Graph graph, char *v, char *output)
{
    int count = 0;
    JRB ptr, find = jrb_find_str(graph.edges, v);
    if (find == NULL)
        return 0;
    else
    {
        JRB tree = (JRB)jval_v(find->val);
        jrb_traverse(ptr, tree)
        {
            // output[count] = strdup(jval_s(ptr->key));
            strcpy(output + count * POS_SIZE, jval_s(ptr->key));
            count++;
        }
    }
    return count;
}

// int *indegree(Graph graph, int v, int *count)
// {
//     *count = 0;
//     int *output = malloc(sizeof(int));
//     JRB ptr;
//     jrb_traverse(ptr, graph.edges)
//     {
//         if (jrb_find_str(jval_v(ptr->val), v) != NULL)
//         {
//             output[*count] = jval_i(ptr->val);
//             (*count)++;
//             output = realloc(output, ((*count) + 1) * sizeof(int));
//         }
//     }
// }

// int DAG(Graph graph)
// {
//     int visited[1000];
//     int n, output[100], i, u, v, start;
//     Dllist node, stack;
//     JRB vertex;

//     jrb_traverse(vertex, graph.vertices)
//     {
//         memset(visited, 0, sizeof(visited));

//         start = jval_i(vertex->key);
//         stack = new_dllist();
//         dll_append(stack, new_jval_i(start));

//         while (!dll_empty(stack))
//         {
//             node = dll_last(stack);
//             u = jval_i(node->val);
//             dll_delete_node(node);
//             if (!visited[u])
//             {
//                 visited[u] = 1;
//                 n = outdegree(graph, u, output);
//                 for (i = 0; i < n; i++)
//                 {
//                     v = output[i];
//                     if (v == start) // cycle detected
//                         return 0;
//                     if (!visited[v])
//                         dll_append(stack, new_jval_i(v));
//                 }
//             }
//         }
//     }
//     return 1; // no cycle
// }

// void TSort(Graph graph)
// {
//     Dllist queue, node;
//     JRB ptr;
//     int n = 0, *indeg, count = 0, v, *output, i, j;
//     indeg = malloc(sizeof(int));
//     jrb_traverse(ptr, graph.vertices)
//     {
//         indeg = calloc(indeg, (n + 1) * sizeof(int));
//         indegree(graph, jval_i(ptr->key), &count);
//         indeg[n] = count;
//         if (count == 0)
//             dll_append(queue, ptr->key);
//         n++;
//     }
//     while (!dll_empty(queue))
//     {
//         node = dll_first(queue);
//         v = jval_i(node->val);
//         dll_delete_node(node);
//         printf("%d\t", v);
//         output = outdegree(graph, v, &count);
//         for (i = 0; i < count; ++i)
//             for (j = 0; j < n; ++j)
//                 if (indeg[j] == output[i])
//                 {
//                     indeg[j]--;
//                     if (indeg[j] == 0)
//                         dll_append(queue, new_jval_i(j));
//                 }
//     }
// }

int findVtx(vtx *v, int n, char *key)
{
    int i;
    for (i = 0; i < n; ++i)
        if (strcmp(v[i].k, key) == 0)
            return i;
}

int findMin(vtx *v, int n)
{
    int i, result = END;
    double min = INFINITIVE;
    for (i = 0; i < n; ++i)
        if (!v[i].t && v[i].d < min)
        {
            min = v[i].d;
            result = i;
        }
    return result;
}

int Route(vtx *v, int min, int hdl, JRB lineTree, vtx *tmp)
{
    int i, result = 0;
    tmp->n_line = 0;
    JRB find;
    for (i = 0; i < v[min].n_line; ++i)
        if ((find = jrb_find_str(lineTree, v[min].line[i])) != NULL)
        {
            strcpy(tmp->line[tmp->n_line++], jval_s(find->key));
            result = 1;
        }
    if (!result)
        jrb_traverse(find, lineTree)
            strcpy(tmp->line[tmp->n_line++], jval_s(find->key));
    return result;
}

double shortestPath(Graph graph, char *s, char *t, char *path, int *length, Guide *guide, int *changeBus)
{
    vtx *v = malloc(sizeof(vtx)), tmp;
    JRB trv, fnd;
    int count = 0, i, hdl, min, nadj, wgt, j, parent;
    char adj[200][POS_SIZE], line[10];
    jrb_traverse(trv, graph.vertices)
    {
        v = realloc(v, (count + 1) * sizeof(vtx));
        strcpy(v[count].k, jval_s(trv->key));
        v[count].d = INFINITIVE;
        v[count].n_line = 0;
        v[count++].t = 0;
    }
    i = findVtx(v, count, s);
    // for (i = 0; i < count; ++i)
    //     if (v[i].k == s)
    //     {
    v[i].d = 0;
    v[i].t = 1;
    // }
    nadj = outdegree(graph, s, &adj[0][0]);
    for (i = 0; i < nadj; ++i)
    {
        hdl = findVtx(v, count, adj[i]);
        trv = jval_v((jrb_find_str(graph.edges, s)->val));
        fnd = jrb_find_str(trv, adj[i]);
        strcpy(v[hdl].p, s);
        fnd = (JRB)jval_v(fnd->val);
        jrb_traverse(trv, fnd)
            strcpy(v[hdl].line[v[hdl].n_line++], jval_s(trv->key));
        v[hdl].d = jval_d(jrb_first(fnd)->val);
    }
    while ((min = findMin(v, count)) != END)
    {
        v[min].t = 1;
        nadj = outdegree(graph, v[min].k, &adj[0][0]);
        for (i = 0; i < nadj; ++i)
        {
            hdl = findVtx(v, count, adj[i]);
            trv = (JRB)jval_v((jrb_find_str(graph.edges, v[min].k)->val));
            fnd = jrb_find_str(trv, adj[i]);
            fnd = (JRB)jval_v(fnd->val);
            if (Route(v, min, hdl, fnd, &tmp))
                wgt = v[min].d + 1;
            else
                wgt = v[min].d + 5;
            if (wgt < v[hdl].d)
            {
                strcpy(tmp.k, v[hdl].k);
                v[hdl] = tmp;
                strcpy(v[hdl].p, v[min].k);
                v[hdl].d = wgt;
            }
        }
    }
    i = findVtx(v, count, t);
    if (v[i].d == INFINITIVE)
        return -1;
    double result = v[i].d;
    *length = 0;
    min = -1;
    guide[min].end = 0;
    parent = i;
    while (strcmp(v[i].k, s) != 0)
    {
        hdl = 0;
        strcpy(line, v[i].line[0]);
        strcpy(path + (*length) * POS_SIZE, v[i].k);
        // printf("%s\n", v[i].line);
        i = findVtx(v, count, v[i].p);
        for (j = 0; j < v[i].n_line; ++j)
            if (strcmp(line, v[i].line[j]) == 0)
                hdl = 1;
        if (!hdl)
        {
            guide[++min].begin = *length + 1;
            for (j = 0; j < v[parent].n_line; ++j)
                strcpy(guide[min].line[guide[min].n_line++], v[parent].line[j]);
            guide[min + 1].end = *length + 1;
            parent = i;
        }
        (*length)++;
    }

    strcpy(path + ((*length)++) * POS_SIZE, s);
    // guide[++min].begin = *length - 1;
    // for (j = 0; j < v[guide[min].end].n_line; ++j)
    //     strcpy(guide[min].line[guide[min].n_line++], v[guide[min].end].line[j]);
    *changeBus = min;
    return result;
}

void readFile(Graph graph, char *file)
{
    FILE *fp = fopen(file, "r");
    char read[30], line[10], pos[POS_SIZE] = "", prv[POS_SIZE] = "";
    JRB find;
    if (fp == NULL)
    {
        printf("Open file failed!\n");
        return;
    }
    while (!feof(fp))
    {
        fscanf(fp, "%s", read);
        if (read[strlen(read) - 1] == ':')
        {
            if (strlen(pos) != 0)
            {
                pos[strlen(pos) - 1] = '\0';
                // printf("%s - %s\n", prv, pos);
                find = jrb_find_str(graph.vertices, pos);
                if (find == NULL)
                    jrb_insert_str(graph.vertices, strdup(pos), new_jval_i(1));
                addEdge(graph, prv, pos, line);
            }
            strcpy(pos, "");
            strcpy(prv, "");
            strncpy(line, read, strlen(read) - 1);
            line[strlen(read) - 1] = '\0';
            // printf("Tuyen\n");
            continue;
        }
        if (strcmp(read, "-") != 0)
        {
            // printf("Them\n");
            strcat(pos, read);
            strcat(pos, " ");
        }
        else
        {
            // printf("Ket thuc\n");
            pos[strlen(pos) - 1] = '\0';
            // printf("%s\n", pos);
            find = jrb_find_str(graph.vertices, pos);
            if (find == NULL)
                jrb_insert_str(graph.vertices, strdup(pos), new_jval_i(1));
            if (strlen(prv) != 0)
                addEdge(graph, prv, pos, line);
            strcpy(prv, pos);
            strcpy(pos, "");
        }
    }
    fclose(fp);
    return;
}
int main()
{
    Guide guide[7];
    Graph busgraph = createGraph();
    JRB ptr;
    readFile(busgraph, "bus2.txt");
    char output[100][POS_SIZE], from[POS_SIZE], to[POS_SIZE];
    int length, i = 0, j = 0, chageBus = 0;
    // jrb_insert_str(busgraph.vertices, "Long Bien", new_jval_i(1));
    // JRB find = jrb_find_str(busgraph.edges, "Diem trung chuyen Long Bien");
    // JRB tree = (JRB)jval_v(find->val);
    // find = jrb_find_str(tree, "Yen Phu");
    // tree = (JRB)jval_v(find->val);
    // jrb_traverse(ptr, tree)
    //     printf("%s\n", jval_s(ptr->key));
    printf("Ban muon di tu: ");
    gets(from);
    printf("Den: ");
    gets(to);
    jrb_traverse(ptr, busgraph.vertices)
    {
        if (!i && strcmp_Ncasesen(from, jval_s(ptr->key)) == 0)
        {
            strcpy(from, jval_s(ptr->key));
            i = 1;
        }
        if (!j && strcmp_Ncasesen(to, jval_s(ptr->key)) == 0)
        {
            strcpy(to, jval_s(ptr->key));
            j = 1;
        }
    }
    if (!i || !j)
    {
        printf("Kiem tra lai dia diem\n");
        return 1;
    }

    if (strcmp(from, to) == 0)
    {
        printf("2 diem giong nhau ma\n");
        return 1;
    }

    double cost = shortestPath(busgraph, from, to, &output[0][0], &length, guide, &chageBus);
    // for (i = length - 1; i >= 0; --i)
    //     printf("%s\n", output[i]);
    if (cost == -1)
    {
        printf("Khong co duong tu <%s> den <%s>\n", from, to);
        return -1;    
    }

    printf("\n\nDuong di\n----------------------------\n");
    for (i = chageBus; i >= 0; --i)
    {
        printf("Bus: ");
        for (j = 0; j < guide[i].n_line; j++)
            printf("%s\t", guide[i].line[j]);
        printf("\n\n");
        for (j = guide[i].begin; j >= guide[i].end; --j)
        {
            printf("%s\n", output[j]);
        }
        printf("----------------------------\n");
    }
    return 0;
}