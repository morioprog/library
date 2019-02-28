/*
・グラフ
  > Dijkstra
  > BellmanFord
  > WarshallFloyd
  > Kruskal
[応用] 単一終点最短路問題は, すべての有向辺を逆向きに張り替えると, 単一始点最短路問題に帰着できる.
[使用例]
Graph<int> g(N);              // 頂点数N, 重さの型がintのグラフを宣言
add_edge(g,a,b,c);            // グラフgに, aからbへの重さcの無向辺を追加
add_arc(g,a,b,c);             // グラフgに, aからbへの重さcの有向辺を追加
add_to_edges(edges,a,b,c);    // 辺集合edgesに, 始点a, 終点b, 重さcの辺を追加
*/

template<typename T> struct Edge {
  int from, to;
  T weight;
  Edge() : from(0), to(0), weight(0) {}
  Edge(int f, int t, T w) : from(f), to(t), weight(w) {}
};
template<typename T> using Edges = vector< Edge< T > >;
template<typename T> using Graph = vector< Edges< T > >;
template<typename T> void     add_edge(Graph< T > &g, int from, int to, T w = 1) { g[from].emplace_back(from,to,w); g[to].emplace_back(to,from,w); }
template<typename T> void      add_arc(Graph< T > &g, int from, int to, T w = 1) { g[from].emplace_back(from,to,w); }
template<typename T> void add_to_edges(Edges< T > &e, int from, int to, T w = 1) { e.emplace_back(from,to,w); }

/*
・ダイクストラ法
  > O(ElogV) [E:辺の数, V:頂点の数]
[備考] 負辺の存在しないグラフに対する単一始点全点間最短路を求めるアルゴリズム
[注意] 結果を足し合わせる際, INFの大きさに注意
[使用例]
auto dij = Dijkstra(g,s);     // グラフgにおける, 始点sからの最短路
*/

template<typename T> vector< T > Dijkstra(Graph<T> &g, int from) {
  const auto INF = numeric_limits< T >::max()/10;
  vector< T > dist(g.size(), INF);
  dist[from] = 0;
  using Pi = pair< T, int >;
  priority_queue< Pi, vector< Pi >, greater< Pi > > que;
  que.emplace(dist[from], from);
  while ( !que.empty() ) {
    T weight; int idx;
    tie(weight, idx) = que.top(); que.pop();
    if (dist[idx] < weight) continue;
    for (auto &e : g[idx]) {
      auto next_weight = weight + e.weight;
      if (dist[e.to] <= next_weight) continue;
      dist[e.to] = next_weight;
      que.emplace(dist[e.to], e.to);
    }
  }
  return dist;
}

/*
・ベルマンフォード法
  > O(EV) [E:辺の数, V:頂点の数]
[備考] グラフ(負辺が存在してもよい)に対する単一始点全点間最短路を求めるアルゴリズム
      負閉路が存在しているかの判定も可能 -> 存在していたら空列を返す
[注意] 結果を足し合わせる際, INFの大きさに注意
[使用例]
Edges<int> edges;                     // 全ての辺 (重さ: int)
add_to_edges(edges,a,b,c);            // 辺集合edgesに, 始点a, 終点b, 重さcの辺を追加
auto bf = BellmanFord(edges,V,s);     // 辺edges, 頂点数Vのグラフにおける, 始点sからの最短路
*/

template<typename T> vector< T > BellmanFord(Edges< T > &edges, int vertex, int from) {
  const auto INF = numeric_limits< T >::max()/10;
  vector< T > dist(vertex, INF);
  dist[from] = 0;
  for (int i = 0; i < vertex - 1; ++i) {
    for (auto &e : edges) {
      if (dist[e.from] == INF) continue;
      dist[e.to] = min(dist[e.to], dist[e.from] + e.weight);
    }
  }
  for (auto &e : edges) {
    if (dist[e.from] == INF) continue;
    if (dist[e.from] + e.weight < dist[e.to]) return vector< T >();
  }
  return dist;
}

/*
・ワーシャルフロイド法
  > O(V^3) [V:頂点数] (WarshallFloyd())
  > O(V^2) : 辺の追加 (add_edge_to_matrix())
[備考] 全点間最短路を求めるアルゴリズム
      負閉路が存在しているかの検出も可能
        -> ある頂点vからv自身への最短路が負なが負閉路が存在
[注意] 結果を足し合わせる際, INFの大きさに注意
[使用例]
auto wf = WarshallFloyd(g);      // グラフgにおける全点間最短路
add_edge_to_matrix(wf,a,b,c);    // 隣接行列wfに, 始点a, 終点b, 重さcの辺を追加して, wfを更新
*/

template<typename T> using Matrix = vector< vector< T > >;
template<typename T> Matrix< T > WarshallFloyd(Graph< T > &g) {
  const auto INF = numeric_limits< T >::max()/10;
  int vertex = g.size();
  Matrix< T > dist(vertex, vector< T >(vertex, INF));
  for (int i = 0; i < vertex; ++i) dist[i][i] = 0;
  for (int i = 0; i < vertex; ++i) {
    for (auto &e : g[i]) {
      dist[e.from][e.to] = min(dist[e.from][e.to], e.weight);
    }
  }
  for (int k = 0; k < vertex; ++k) {
    for (int i = 0; i < vertex; ++i) {
      for (int j = 0; j < vertex; ++j) {
        if (dist[i][k] != INF && dist[k][j] != INF) {
          dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
        }
      }
    }
  }
  return dist;
}
template<typename T> void add_edge_to_matrix(Matrix< T > &mat, int from, int to, T weight = 1) {
  const auto INF = numeric_limits< T >::max()/10;
  mat[from][to] = mat[to][from] = min(mat[from][to], weight);
  int vertex = mat.size();
  for (int k : {from, to}) {
    for (int i = 0; i < vertex; ++i) {
      for (int j = 0; j < vertex; ++j) {
        if (mat[i][k] != INF && mat[k][j] != INF) {
          mat[i][j] = min(mat[i][j], mat[i][k] + mat[k][j]);
        }
      }
    }
  }
}

/*
・Kruskal法
  > O(ElogV) [E:辺の数, V:頂点の数]
[備考] 最小全域木を求めるアルゴリズム.
      UnionFindで, 閉路を作らないように辺を重みが小さい順に加えていく.
[注意] UnionFindを上で定義しておくこと.
[使用例]
Edges<int> edges;                     // 全ての辺 (重さ: int)
add_to_edges(edges,a,b,c);            // 辺集合edgesに, 始点a, 終点b, 重さcの辺を追加
auto k = Kruskal(edges,V);            // 辺edges, 頂点数Vのグラフにおける最小全域木の重さ
*/

template<typename T> T Kruskal(Edges< T > &edges, int V) {
  sort(begin(edges), end(edges), [](const Edge< T > &a, const Edge< T > &b) {
    return (a.weight < b.weight);
  });
  UnionFind tree(V);
  T ret = 0;
  for (auto & e : edges) {
    if (tree.unite(e.from, e.to)) ret += e.weight;
  }
  return ret;
}

/*
・トポロジカルソート
  > O(E+V) [E:辺の数, V:頂点の数]
[備考] グラフの位相的順序を求める.
[使用例]
vector<int> ts;             // トポロジカルソートの結果を格納するvector.
TopologicalSort(g, ts);     // グラフgのトポロジカルソートの結果をtsに格納. 返り値はトポロジカルソート可能か否か.
*/

template<typename T> bool TopologicalSort(const Graph<T> &g, vector<int> &order) {
  int V = g.size();
  vector<int> color(V);
  function<bool(int)> visit = [&](int v) {
    color[v] = 1;
    for (auto& e: g[v]) {
      if (color[e.to] == 2) continue;
      if (color[e.to] == 1) return false;
      if (!visit(e.to)) return false;
    }
    order.push_back(v); color[v] = 2;
    return true;
  };
  for (int i = 0; i < V; ++i) if (!color[i] && !visit(i)) return false;
  reverse(order.begin(), order.end());
  return true;
}

/*
・二部グラフの判定
  > O(E+V) [E:辺の数, V:頂点の数]
[備考] 二部グラフならTrueを返す.
[使用例] isBipartiteGraph(g);
[その他] 二部グラフだった際に, 片方の集合の大きさが知りたい場合は, return retを削除して, intを返すようにする.
        その場合は, 二部グラフならばその集合の大きさを, そうでない場合は-1を返す.
*/

template <typename T> bool isBipartiteGraph(const Graph<T> &g) {
  int V = g.size();
  vector<int> color(V, 0);
  bool ret = true;
  function<void(int, int)> dfs = [&](int i, int clr) {
    if (color[i] != 0) return;
    color[i] = clr;
    for (auto& e: g[i]) {
      if (color[e.to] == 0) dfs(e.to, -clr);
      else if (color[e.to] == clr) ret = false;
    }
  };
  dfs(0, 1);
  return ret;
  int cnt = 0;
  for (auto& e: color) if (e == 1) ++cnt;
  return ret ? -1 : cnt;
}
