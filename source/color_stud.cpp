#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <time.h>
#include <limits>
using namespace std;

#define INDIR "../../test_packages/"

class ColoringProblem
{
public:
    void ReadGraphFile(string filename)
    {
        ifstream fin(INDIR + filename);
        string line;
        int vertices = 0, edges = 0;
        while (getline(fin, line))
        {
            if (line[0] == 'c')
            {
                continue;
            }

            stringstream line_input(line);
            char command;
            if (line[0] == 'p')
            {
                string type;
                line_input >> command >> type >> vertices >> edges;
                neighbour_sets.resize(vertices);
                colors.resize(vertices);
                for (auto& c : colors)
                {
                    c = -1;
                }
            }
            else
            {
                int start, finish;
                line_input >> command >> start >> finish;
                // Edges in DIMACS file can be repeated, but it is not a problem for our sets
                neighbour_sets[start - 1].insert(finish - 1);
                neighbour_sets[finish - 1].insert(start - 1);
            }
        }
    }

    void GreedyGraphColoring()
    {
        vector<int> uncolored_vertices(neighbour_sets.size());
        vector<pair<int, int>> degree_vertices(neighbour_sets.size());
        for (size_t i = 0; i < uncolored_vertices.size(); ++i)
        {
            degree_vertices[i] = make_pair<int, int>(neighbour_sets[i].size(), i);
        }
        std::sort(degree_vertices.begin(), degree_vertices.end(), 
                 [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                     return a.first < b.first;
                 });

        int color = 1;
        int count = 0;

        // per color
        while (count < neighbour_sets.size())
        {
            for (int i = degree_vertices.size()-1; i >= 0; --i)
            {
                int vertex = degree_vertices[i].second;
                if (colors[vertex] == -1)
                {
                    bool iscolored = true;
                    for (int neighbour : neighbour_sets[vertex])
                    {
                        if (colors[neighbour] == color)
                        {
                            iscolored = false;
                            break;
                        }
                    }
                    if (iscolored)
                    {
                        colors[vertex] = color;
                        count++;
                    }
                }
            }
            color = ++maxcolor;
        }

        maxcolor -= 1;
    }

    bool Check()
    {
        for (size_t i = 0; i < neighbour_sets.size(); ++i)
        {
            if (colors[i] == 0)
            {
                cout << "Vertex " << i + 1 << " is not colored\n";
                return false;
            }
            for (int neighbour : neighbour_sets[i])
            {
                if (colors[neighbour] == colors[i])
                {
                    cout << "Neighbour vertices " << i + 1 << ", " << neighbour + 1 <<  " have the same color\n";
                    return false;
                }
            }
        }
        return true;
    }

    int GetNumberOfColors()
    {
        return maxcolor;
    }

    const vector<int>& GetColors()
    {
        return colors;
    }

private:
    vector<int> colors;
    int maxcolor = 1;
    vector<unordered_set<int>> neighbour_sets;
};

int main()
{
    vector<string> files = { "myciel3.col" , "myciel7.col", "latin_square_10.col", "school1.col", "school1_nsh.col",
        "mulsol.i.1.col", "inithx.i.1.col", "anna.col", "huck.col", "jean.col", "miles1000.col", "miles1500.col",
        "fpsol2.i.1.col", "le450_5a.col", "le450_15b.col", "le450_25a.col", "games120.col",
        "queen11_11.col", "queen5_5.col" };
    // vector<string> files = { "queen11_11.col" };
    ofstream fout("../color.csv");
    fout << "Instance; Colors; Time (sec)\n";
    cout << "Instance; Colors; Time (sec)\n";
    for (string file : files)
    {
        ColoringProblem problem;
        problem.ReadGraphFile(file);
        clock_t start = clock();
        problem.GreedyGraphColoring();
        if (! problem.Check())
        {
            fout << "*** WARNING: incorrect coloring: ***\n";
            cout << "*** WARNING: incorrect coloring: ***\n";
        }
        fout << file << "; " << problem.GetNumberOfColors() << "; " << double(clock() - start) / 1000 << '\n';
        cout << file << "; " << problem.GetNumberOfColors() << "; " << double(clock() - start) / 1000 << '\n';

        // for (auto &it: problem.GetColors())
        // {
        //     cout << it << ",\t";
        // }

        // return 0;
    }
    fout.close();
    return 0;
}