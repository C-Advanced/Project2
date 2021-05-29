#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "libfdr/dllist.h"
#include "libfdr/jrb.h"
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
    char line[10][10]; //lưu các tuyến bus để đến đỉnh này
    int n_line;        //số các tuyến bus để đi đến đỉnh này (vì có thể có nhiều chuyến)
    char k[POS_SIZE];  //tên đỉnh
    char p[POS_SIZE];  //đỉnh cha                       |
    double d;          //trọng số đường đi             |   Dung cho dijiktra
    int t;             //đã thăm hay chưa(0: chưa thăm)|
} vtx;

typedef struct         //lưu thông tin hành trình từ <begin> đến <end> có <n_line> tuyến bus được lưu trong <line[i]> 
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


int strcmp_Ncasesen(char *s1, char *s2) //strcmp không phân biệt chữ hoa
{
    int len1 = strlen(s1), len2 = strlen(s2), i;
    if (len1 != len2)
        return 1;
    for (i = 0; i < len1; ++i)
        if (tolower(s1[i]) != tolower(s2[i]))
            return 1;
    return 0;
}

void addEdge(Graph g, char *pos1, char *pos2, char *lineval)  //pos1, 2: đỉnh 1, 2; lineval: tên chuyến bus có thể đi từ <pos1> -> <pos2>
{
    JRB ptr;
    JRB Node = jrb_find_str(g.edges, pos1);                  //cây edge lưu key là các đỉnh, val là jrb (Tạm gọi jrb_edge_val)
    JRB tree, line;
    int i = 0;
    if (Node == NULL)
    {
        tree = make_jrb();                                   //tree là jrb_edge_val, có key là tên đỉnh kề, val là jrb (tạm gọi jrb_edge_val_val)
        line = make_jrb();					//line là jrb_edge_val_val tức node->val của tree
        jrb_insert_str(line, strdup(lineval), new_jval_i(1));  //insert tên chuyến, độ dài măc định là 1
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

int hasEdge(Graph graph, char *pos1, char *pos2) //kiểm tra cạnh nối 2 đỉnh
{
    JRB find = jrb_find_str(graph.edges, pos1);
    if (find == NULL)
        return 0;
    if (jrb_find_str(jval_v(find->val), pos2) != NULL)
        return 1;
    return 0;
}

//lấy giá trị của cạnh
double getEdgeValue(Graph graph, char *pos1, char *pos2)
{
    JRB find = jrb_find_str(graph.edges, pos1), edge;
    if (find == NULL)
        return INFINITIVE;
    edge = jrb_find_str(jval_v(find->val), pos2);
    if (edge == NULL)
        return INFINITIVE;
    return 1;
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
            strcpy(output + count * POS_SIZE, jval_s(ptr->key));
            count++;
        }
    }
    return count;
}

//hàm tìm chỉ số đỉnh <key> trong mảng vertex[]
int findVtx(vtx *v, int n, char *key)          //n: số đỉnh
{
    int i;
    for (i = 0; i < n; ++i)
        if (strcmp(v[i].k, key) == 0)
            return i;
}

//hàm tìm đỉnh có giá trị đường đi nhỏ nhất trong mảng vertex[]
//(dùng cho dijiktra, tương tự queue)
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

//hàm kiểm tra có phải chuyển bus khi đi từ v[min] đến đỉnh kế tiếp không
//đọc hàm dưới để hiểu hơn
int Route(vtx *v, int min, JRB lineTree, vtx *tmp)            //lineTree lưu tất cả tuyến bus từ v[min] -> đỉnh kế tiếp
{
    int i, result = 0;
    tmp->n_line = 0;
    JRB find;
    for (i = 0; i < v[min].n_line; ++i)                                //duyệt tất cả tuyến bus dùng để đến v[min]
        if ((find = jrb_find_str(lineTree, v[min].line[i])) != NULL)
        {
            strcpy(tmp->line[tmp->n_line++], jval_s(find->key));       //nếu có tuyến bus đi qua v[min] và qua cả đỉnh kế tiếp thì dùng tuyến đó để đi tiếp
            result = 1;
        }
    if (!result)                                                      //các tuyến bus qua v[min] không qua đỉnh kế (bắt buộc chuyển bus)
        jrb_traverse(find, lineTree)
            strcpy(tmp->line[tmp->n_line++], jval_s(find->key));
    return result;
}

//dijiktra
double shortestPath(Graph graph, char *s, char *t, char *path, int *length, Guide *guide, int *changeBus)
{
    vtx *v = malloc(sizeof(vtx)), tmp;
    JRB trv, fnd;
    int count = 0, i, hdl, min, nadj, wgt, j, parent;
    char adj[200][POS_SIZE], line[10];
    
    //khởi tạo mảng vtx
    jrb_traverse(trv, graph.vertices)
    {
        v = realloc(v, (count + 1) * sizeof(vtx));
        strcpy(v[count].k, jval_s(trv->key));
        v[count].d = INFINITIVE;
        v[count].n_line = 0;
        v[count++].t = 0;
    }
    
    i = findVtx(v, count, s); //tìm chỉ số đỉnh <s> trong vtx[]
    v[i].d = 0;
    v[i].t = 1;
    
    nadj = outdegree(graph, s, &adj[0][0]);
    for (i = 0; i < nadj; ++i)
    {
        hdl = findVtx(v, count, adj[i]);                        //hdl: handle - chỉ số đỉnh đang thao tác
        trv = jval_v((jrb_find_str(graph.edges, s)->val));
        fnd = jrb_find_str(trv, adj[i]);
        strcpy(v[hdl].p, s);
        fnd = (JRB)jval_v(fnd->val);                            //fnd: jrb_edge_val_val
        jrb_traverse(trv, fnd)
            strcpy(v[hdl].line[v[hdl].n_line++], jval_s(trv->key));   //copy các tuyến bus có thể đi từ s -> v[hdl]
        v[hdl].d = jval_d(jrb_first(fnd)->val);                       //lấy giá trị đường đi
    }
    
    while ((min = findMin(v, count)) != END)         // == while (queue != rỗng); dequeue(queue);
    {
        v[min].t = 1;                               //đánh dấu đã thăm
        nadj = outdegree(graph, v[min].k, &adj[0][0]);
        for (i = 0; i < nadj; ++i)
        {
            hdl = findVtx(v, count, adj[i]);
            trv = (JRB)jval_v((jrb_find_str(graph.edges, v[min].k)->val));
            fnd = jrb_find_str(trv, adj[i]);
            fnd = (JRB)jval_v(fnd->val);
            if (Route(v, min, fnd, &tmp))                 //kiểm tra xem có thể đi từ v[min]->v[hdl] mà không cần chuyển tuyến không
                wgt = v[min].d + 1;
            else
                wgt = v[min].d + 5;                       //nếu phải chuyển tuyến, độ dài đường += 5 (tương đương đi thêm 4 chặng), giá trị 5 là tự đặt
            if (wgt < v[hdl].d)                           
            {
                strcpy(tmp.k, v[hdl].k);
                v[hdl] = tmp;                           // cập nhật lại đỉnh nếu tìm được đường đi ngắn hơn
                strcpy(v[hdl].p, v[min].k);
                v[hdl].d = wgt;
            }
        }
    }
    
    i = findVtx(v, count, t);
    
    if (v[i].d == INFINITIVE)                        // nếu không tồn tại đường đi từ <s> tới <t>
        return -1;
        
    double result = v[i].d;
    *length = 0;
    min = -1;
    guide[min].end = 0;                              //end của chuyến bus cuối là <t>, tức path[0]
    parent = i;
    
    //tìm ngược lại cách đi từ <s> tới <t>
    while (strcmp(v[i].k, s) != 0)
    {
        hdl = 0;
        strcpy(line, v[i].line[0]);                    //lấy chuyến bus đầu tiên để đi đến đỉnh đang xét v[i]
        strcpy(path + (*length) * POS_SIZE, v[i].k);   //cập nhật lộ trình (path lưu hành trình đi từ <s> đến <t>)
        // printf("%s\n", v[i].line);
        i = findVtx(v, count, v[i].p);                 //tìm đỉnh cha của v[i]
        
        //i lúc này đã lưu chỉ số đỉnh cha, <line> lưu 1 chuyến bus để đi từ v[i] hiện tại (cha) đến v[i] phía trên(con)
        for (j = 0; j < v[i].n_line; ++j)                 //kiểm tra chuyến bus <line> có dùng để đi đến v[i] hiện tại không
            if (strcmp(line, v[i].line[j]) == 0)          //nếu có tức không cần chuyển bus, không tức là phải chuyển bus
                hdl = 1;            
        
        //nếu phải chuyển bus
        if (!hdl)
        {
            guide[++min].begin = *length + 1;           //đặt điểm bắt đầu của tuyến bus trên lộ trình <path>
            for (j = 0; j < v[parent].n_line; ++j)
                strcpy(guide[min].line[guide[min].n_line++], v[parent].line[j]);   //lưu tên các tuyến bus
            guide[min + 1].end = *length + 1;           //điểm bắt đầu của tuyến bus này là điểm kết thúc của chuyến bus trước đó
            parent = i;                                 //parent lưu chỉ số điểm cuối của tuyến bus
        }
        (*length)++;                                    //lưu số điểm đi qua trên toàn lộ trình
    }

    strcpy(path + ((*length)++) * POS_SIZE, s);  //cập nhật điểm đầu hành trình vào lộ trình

    *changeBus = min;                                  //số lần chuyển bus
    return result;
}

void readFile(Graph graph, char *file) //Đọc file
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
