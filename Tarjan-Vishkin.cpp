#include "bits/stdc++.h"
using namespace std;

// This is a simple implementation of the Tarjan-Vishkin algorithm
// for computing the strongly connected components of a directed graph.
// The algorithm is described in the paper: https://doi.org/10.1137/0214061

/*
Finding Biconnected Components in a Undirected Graph
*/
std::vector<long long int> dsu_parent, dsu_rank;

void make_set(long long int v)
{
    dsu_parent[v] = v;
    dsu_rank[v] = 0;
}

int find_set(long long int v)
{
    if (v == dsu_parent[v])
        return v;
    return dsu_parent[v] = find_set(dsu_parent[v]);
}

void union_sets(long long int a, long long int b)
{
    a = find_set(a);
    b = find_set(b);
    if (a != b)
    {
        if (dsu_rank[a] < dsu_rank[b])
            swap(a, b);
        dsu_parent[b] = a;
        if (dsu_rank[a] == dsu_rank[b])
            dsu_rank[a]++;
    }
}

struct vertex
{
    long long int id;
    long long int low;
    long long int high;
    long long int descendant_count;
    long long int discovery_time;
    long long int component_number;
    long long int parent_id;
    long long int parent_edge_number;
    bool visited;
    bool root;
};

struct edge
{
    long long int id;
    struct vertex *vertex_1;
    struct vertex *vertex_2;
    bool visited;
    bool Tree_edge;
    long long int component_number;
};

vector<struct vertex *> vertices;
vector<struct edge *> edges;
vector<vector<pair<long long int, long long int>>> vertexAdjacencyList;
vector<vector<long long int>> edgeAdjacencyList;

long long int DFS_timer = 0;

void input(long long int vertexCount, long long int edgeCount)
{

    dsu_parent.resize(edgeCount);
    dsu_rank.resize(edgeCount);
    for (long long int i = 0; i < vertexCount; i++)
    {
        struct vertex *temp_vertex = new vertex;
        temp_vertex->id = i;
        temp_vertex->low = 0;
        temp_vertex->high = 0;
        temp_vertex->descendant_count = 0;
        temp_vertex->discovery_time = 0;
        temp_vertex->component_number = 0;
        temp_vertex->visited = false;
        temp_vertex->root = false;
        temp_vertex->parent_id = -1;
        vertices.push_back(temp_vertex);
    }

    long long int vertex_1, vertex_2;
    for (long long int i = 0; i < edgeCount; i++)
    {

        cin >> vertex_1 >> vertex_2;

        struct edge *temp_edge = new edge;
        temp_edge->id = i;
        temp_edge->vertex_1 = vertices[vertex_1];
        temp_edge->vertex_2 = vertices[vertex_2];
        temp_edge->visited = false;
        temp_edge->Tree_edge = false;
        edges.push_back(temp_edge);
        vertexAdjacencyList[vertex_1].push_back(make_pair(vertex_2, i));
        vertexAdjacencyList[vertex_2].push_back(make_pair(vertex_1, i));
        make_set(i);
    }
}

void DFS(long long int vertex_id, long long int component_number)
{
    vertices[vertex_id]->visited = true;
    vertices[vertex_id]->component_number = component_number;
    vertices[vertex_id]->discovery_time = DFS_timer;
    vertices[vertex_id]->low = DFS_timer;
    vertices[vertex_id]->high = DFS_timer;
    vertices[vertex_id]->descendant_count = 1;
    DFS_timer++;
    for (pair<long long int, long long int> to_edge : vertexAdjacencyList[vertex_id])
    {
        long long int TO = to_edge.first;
        long long int TO_edge_id = to_edge.second;
        if (TO == vertices[vertex_id]->parent_id)
            continue;
        if (vertices[TO]->visited)
        {
            vertices[vertex_id]->low = min(vertices[vertex_id]->low, vertices[TO]->discovery_time);
            vertices[vertex_id]->high = max(vertices[vertex_id]->high, vertices[TO]->discovery_time);
        }
        else
        {
            vertices[TO]->parent_id = vertex_id;
            vertices[TO]->parent_edge_number = TO_edge_id;
            DFS(TO, component_number);
            vertices[vertex_id]->low = min(vertices[vertex_id]->low, vertices[TO]->low);
            vertices[vertex_id]->high = max(vertices[vertex_id]->high, vertices[TO]->high);
            vertices[vertex_id]->descendant_count += vertices[TO]->descendant_count;
            edges[TO_edge_id]->Tree_edge = true;
        }
    }
}

void graph_traversal(long long int vertex_count)
{

    long long int component_number = 0;
    long long int *discovery_time = 0;
    for (long long int i = 0; i < vertex_count; i++)
    {
        if (vertices[i]->visited == false)
        {
            DFS(i, component_number);
            component_number++;
            vertices[i]->root = true;
        }
    }
}

void build_auxiliary_graph(long long int vertex_count, long long int edge_count)
{

    for (long long int i = 0; i < edge_count; i++)
    {
        if (edges[i]->Tree_edge)
        {

            long long int parent_vertex, child_vertex;
            if (edges[i]->vertex_1->parent_id == edges[i]->vertex_2->id)
            {
                parent_vertex = edges[i]->vertex_2->id;
                child_vertex = edges[i]->vertex_1->id;
            }
            else
            {
                parent_vertex = edges[i]->vertex_2->id;
                child_vertex = edges[i]->vertex_1->id;
            }

            for (pair<long long int, long long int> to_edge : vertexAdjacencyList[child_vertex])
            {
                if (to_edge.first == parent_vertex)
                    continue;
                if (edges[to_edge.second]->Tree_edge)
                    continue;

                long long int back_edge_vertex;
                if (edges[to_edge.second]->vertex_1->id == child_vertex)
                    back_edge_vertex = edges[to_edge.second]->vertex_2->id;
                else
                    back_edge_vertex = edges[to_edge.second]->vertex_1->id;

                if (vertices[back_edge_vertex]->discovery_time < vertices[child_vertex]->discovery_time)
                {
                    union_sets(i, to_edge.second);
                    edgeAdjacencyList[i].push_back(to_edge.second);
                    edgeAdjacencyList[to_edge.second].push_back(i);
                }
            }

            if (vertices[parent_vertex]->root == true)
                continue;
            if ((vertices[child_vertex]->low < vertices[parent_vertex]->discovery_time) ||
                (vertices[child_vertex]->high > (vertices[parent_vertex]->discovery_time + vertices[parent_vertex]->descendant_count)))
            {
                long long int second_edge_id = vertices[parent_vertex]->parent_edge_number;
                union_sets(i, second_edge_id);
                edgeAdjacencyList[i].push_back(second_edge_id);
                edgeAdjacencyList[second_edge_id].push_back(i);
            }
        }
        else
        {
            long long int vertex_1 = edges[i]->vertex_1->id;
            long long int vertex_2 = edges[i]->vertex_2->id;
            if((vertices[vertex_1]->discovery_time + vertices[vertex_1]->descendant_count <= vertices[vertex_2]->discovery_time)||
                (vertices[vertex_2]->discovery_time + vertices[vertex_2]->descendant_count <= vertices[vertex_1]->discovery_time))
            {
                long long int tree_edge_1 = vertices[vertex_1]->parent_edge_number;
                long long int tree_edge_2 = vertices[vertex_2]->parent_edge_number;
                union_sets(tree_edge_1, tree_edge_2);
                edgeAdjacencyList[tree_edge_1].push_back(tree_edge_2);
                edgeAdjacencyList[tree_edge_2].push_back(tree_edge_1);
            }
        }
    }
}
int main()
{
    long long int vertexCount, edgeCount;
    cin >> vertexCount >> edgeCount;
    input(vertexCount, edgeCount);
    graph_traversal(vertexCount);
    build_auxiliary_graph(vertexCount, edgeCount);
}