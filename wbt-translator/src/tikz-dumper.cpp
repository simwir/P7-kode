#include <fstream>
#include <iostream>

#include "wbt-translator/webots_parser.hpp"

using namespace std;

struct Node {
    Waypoint &wp;
};

struct Adjlist {
    Waypoint &wp;
};

inline char to_char(int i)
{
    return char(i + 'a');
}

ostream &operator<<(ostream &os, const WaypointType wpt)
{
    switch (wpt) {
    case WaypointType::eVia:
        return os << "via";
    case WaypointType::eStation:
        return os << "station";
    case WaypointType::eEndPoint:
        return os << "endpoint";
    }
}

ostream &operator<<(ostream &os, const Translation &pt)
{
    return os << "(\\w*" << pt.x << ",\\h*" << -pt.z << ")";
}

ostream &operator<<(ostream &os, const Node &node)
{
    return os << "\\node[" << node.wp.waypointType << "] (" << to_char(node.wp.id) << ") at "
              << node.wp.translation << " {" << node.wp.id << "};\n";
}

ostream &operator<<(ostream &os, const Adjlist &node)
{
    for (int adj : node.wp.adjlist) {
        os << "\\draw (" << to_char(adj) << ") edge (" << to_char(node.wp.id) << ");\n";
    }
    return os;
}

int main()
{
    ifstream is("../../webot/worlds/testmap_1.wbt");
    Parser parser{is};
    AST ast = parser.parse_stream();

    /*    cout << "\\begin{tikzpicture}[->,>=stealth']\n"
         << "\tikzset{station/.style = {state,square}};\n"
         << "\tikzset{via/.style={state,triangle}};\n"
         << "\tikzset{endpoint/.style={state}}n"
         << "\\def\\w{1.5}\\h{1.5}\n";*/
    for (auto &[_, node] : ast.nodes) {
        cout << Node{node};
    }
    for (auto &[_, node] : ast.nodes) {
        cout << Adjlist{node};
    }
    //cout << "\\end{tikzpicture}" << std::endl;

    return 0;
}
