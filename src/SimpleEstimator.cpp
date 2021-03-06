#include "SimpleGraph.h"
#include "SimpleEstimator.h"
#include "SimpleEvaluator.h"
#include <set>
#include <cmath>
#include <ctime>


/////
///// Stats class
/////
Stats::Stats(uint32_t noLabels, uint32_t noVertices) {
    labels = noLabels;
    vertices = noVertices;

    total_relations.push_back({});
    distinct_source_relations.push_back({});
    distinct_target_relations.push_back({});
}

void Stats::create_stats(std::shared_ptr<SimpleGraph> *g) {
    source_relations_count = std::vector<std::vector<uint32_t >> (labels, std::vector<uint32_t > (vertices));
    target_relations_count = std::vector<std::vector<uint32_t >> (labels, std::vector<uint32_t > (vertices));
    distinct_source_relations = std::vector<uint32_t > (labels);
    distinct_target_relations = std::vector<uint32_t > (labels);

    for (uint32_t rel_type = 0; rel_type < labels; rel_type++) {
        for (uint32_t vertice = 0; vertice < vertices; vertice++) {
            if (source_relations_count[rel_type][vertice] > 0) {
                distinct_source_relations[rel_type]++;
            }
            if (target_relations_count[rel_type][vertice] > 0)
                distinct_target_relations[rel_type]++;
        }
    }

    // matrix[rel_label_i][rel_label_j][rel_type_i][rel_type_j] = {tuples, source_dist, middle_dist, final_dist}
    multidimensional_matrix = std::vector<std::vector<std::vector<std::vector<std::vector<uint32_t>>>>> (labels,
        std::vector<std::vector<std::vector<std::vector<uint32_t>>>> (labels,
            std::vector<std::vector<std::vector<uint32_t>>> (2,
                std::vector<std::vector<uint32_t>> (2,
                    std::vector<uint32_t> (4, 0)))));

    uint32_t subject; // subject id
    uint32_t x_step_i; // start subject
    uint32_t x_step_j; // end subject
    uint32_t y_step_i; // start subject
    uint32_t y_step_j; // end subject
    uint32_t V = (*g)->getNoVertices();

    uint32_t tuples;
    std::unordered_set<uint32_t> source_answers;
    uint32_t middle_answers;
    std::unordered_set<uint32_t> final_answers;
    uint32_t x_target;
    uint32_t s;
    uint32_t m;
    uint32_t f;
    uint32_t source_answers_size;
    std::vector<uint32_t> result;

    for (uint32_t rel_x = 0; rel_x < labels; rel_x++) {
        for (uint32_t rel_y = 0; rel_y < rel_x+1; rel_y++) {
            for (uint32_t x_normal = 0; x_normal < (uint32_t) 2; x_normal++) {
                for (uint32_t y_normal = 0; y_normal < (uint32_t)2; y_normal++) {
                    tuples = 0;
                    source_answers = {};
                    middle_answers = 0;
                    final_answers = {};

                    if ((rel_x == rel_y) && (x_normal != y_normal)) {
                        if (x_normal == 0){
                            // counter++;counter++;
                            x_step_i = (*g)->positions_adj_reverse[rel_x][0];
                            x_step_j = (*g)->positions_adj_reverse[rel_x][1];
                            y_step_i = (*g)->positions_adj_reverse[rel_y][0];
                            y_step_j = (*g)->positions_adj_reverse[rel_y][1];
                            for (uint32_t joinSubjectJ = 1; joinSubjectJ < V; joinSubjectJ++) {
                                if ((x_step_i != x_step_j) && (y_step_i != y_step_j)) {
                                    middle_answers++;
                                    tuples += (x_step_j-x_step_i) * (y_step_j-y_step_i);
                                    while (x_step_i < x_step_j) {
                                        source_answers.insert((*g)->IA_reverse[x_step_i]);
                                        x_step_i++;
                                    }
                                }
                                x_step_i = x_step_j;
                                y_step_i = y_step_j;
                                x_step_j = (*g)->positions_adj_reverse[rel_x][joinSubjectJ];
                                y_step_j = (*g)->positions_adj_reverse[rel_y][joinSubjectJ];
                            }
                            s = source_answers.size();
                            m = middle_answers;

                            multidimensional_matrix[rel_x][rel_x][0][1] = {tuples, s, m, s};
                            multidimensional_matrix[rel_x][rel_x][1][0] = {tuples, s, m, s};
                        }
                    } else {
                        if ((x_normal == 1) && (y_normal == 0)) {
                            x_step_i = (*g)->positions_adj[rel_x][0];
                            x_step_j = (*g)->positions_adj[rel_x][1];
                            y_step_i = (*g)->positions_adj[rel_y][0];
                            y_step_j = (*g)->positions_adj[rel_y][1];
                            for (uint32_t joinSubjectJ = 1; joinSubjectJ < V; joinSubjectJ++) {
                                if ((x_step_i != x_step_j) && (y_step_i != y_step_j)) {
                                    middle_answers++;
                                    tuples += (x_step_j-x_step_i) * (y_step_j-y_step_i);
                                    while (x_step_i < x_step_j) {
                                        source_answers.insert((*g)->IA[x_step_i]);
                                        x_step_i++;
                                    }
                                    while (y_step_i < y_step_j) {
                                        final_answers.insert((*g)->IA[y_step_i]);
                                        y_step_i++;
                                    }
                                }
                                x_step_i = x_step_j;
                                y_step_i = y_step_j;
                                x_step_j = (*g)->positions_adj[rel_x][joinSubjectJ];
                                y_step_j = (*g)->positions_adj[rel_y][joinSubjectJ];
                            }
                        } else if ((x_normal == 0) && (y_normal == 0)) {
                            x_step_i = (*g)->positions_adj_reverse[rel_x][0];
                            x_step_j = (*g)->positions_adj_reverse[rel_x][1];
                            y_step_i = (*g)->positions_adj[rel_y][0];
                            y_step_j = (*g)->positions_adj[rel_y][1];
                            for (uint32_t joinSubjectJ = 1; joinSubjectJ < V; joinSubjectJ++) {
                                if ((x_step_i != x_step_j) && (y_step_i != y_step_j)) {
                                    middle_answers++;
                                    tuples += (x_step_j-x_step_i) * (y_step_j-y_step_i);
                                    while (x_step_i < x_step_j) {
                                        source_answers.insert((*g)->IA_reverse[x_step_i]);
                                        x_step_i++;
                                    }
                                    while (y_step_i < y_step_j) {
                                        final_answers.insert((*g)->IA[y_step_i]);
                                        y_step_i++;
                                    }
                                }
                                x_step_i = x_step_j;
                                y_step_i = y_step_j;
                                x_step_j = (*g)->positions_adj_reverse[rel_x][joinSubjectJ];
                                y_step_j = (*g)->positions_adj[rel_y][joinSubjectJ];
                            }
                        } else if ((x_normal == 1) && (y_normal == 1)) {
                            x_step_i = (*g)->positions_adj[rel_x][0];
                            x_step_j = (*g)->positions_adj[rel_x][1];
                            y_step_i = (*g)->positions_adj_reverse[rel_y][0];
                            y_step_j = (*g)->positions_adj_reverse[rel_y][1];
                            for (uint32_t joinSubjectJ = 1; joinSubjectJ < V; joinSubjectJ++) {
                                if ((x_step_i != x_step_j) && (y_step_i != y_step_j)) {
                                    middle_answers++;
                                    tuples += (x_step_j-x_step_i) * (y_step_j-y_step_i);
                                    while (x_step_i < x_step_j) {
                                        source_answers.insert((*g)->IA[x_step_i]);
                                        x_step_i++;
                                    }
                                    while (y_step_i < y_step_j) {
                                        final_answers.insert((*g)->IA_reverse[y_step_i]);
                                        y_step_i++;
                                    }
                                }
                                x_step_i = x_step_j;
                                y_step_i = y_step_j;
                                x_step_j = (*g)->positions_adj[rel_x][joinSubjectJ];
                                y_step_j = (*g)->positions_adj_reverse[rel_y][joinSubjectJ];
                            }
                        } else {
                            x_step_i = (*g)->positions_adj_reverse[rel_x][0];
                            x_step_j = (*g)->positions_adj_reverse[rel_x][1];
                            y_step_i = (*g)->positions_adj_reverse[rel_y][0];
                            y_step_j = (*g)->positions_adj_reverse[rel_y][1];
                            for (uint32_t joinSubjectJ = 1; joinSubjectJ < V; joinSubjectJ++) {
                                if ((x_step_i != x_step_j) && (y_step_i != y_step_j)) {
                                    middle_answers++;
                                    tuples += (x_step_j-x_step_i) * (y_step_j-y_step_i);
                                    while (x_step_i < x_step_j) {
                                        source_answers.insert((*g)->IA_reverse[x_step_i]);
                                        x_step_i++;
                                    }
                                    while (y_step_i < y_step_j) {
                                        final_answers.insert((*g)->IA_reverse[y_step_i]);
                                        y_step_i++;
                                    }
                                }
                                x_step_i = x_step_j;
                                y_step_i = y_step_j;
                                x_step_j = (*g)->positions_adj_reverse[rel_x][joinSubjectJ];
                                y_step_j = (*g)->positions_adj_reverse[rel_y][joinSubjectJ];
                            }
                        }

                        s = source_answers.size();
                        m = middle_answers;
                        f = final_answers.size();

                        multidimensional_matrix[rel_x][rel_y][x_normal][y_normal] = {tuples, s, m, f};
                        multidimensional_matrix[rel_y][rel_x][1-y_normal][1-x_normal] = {tuples, f, m, s};
                    }
                }
            }
        }
    }
}

std::vector<uint32_t> get_relation_info(std::string relation) { // path[0]
    std::vector<uint32_t> relation_info;

    // relation label
    uint32_t T = std::stoi(relation.substr(0, relation.size()-1));
    relation_info.push_back(T);

    // relation direction
    std::string dir = relation.substr(relation.size()-1, 1);
    relation_info.push_back(uint32_t(dir != ">") + uint32_t(dir == "*")); // 0:>; 1:<; 2:+;

    return relation_info;
}

uint32_t SimpleEstimator::get_in(std::vector<uint32_t> relation_info) {
    if (relation_info[1] == 0) {
        return stats.distinct_target_relations[relation_info[0]];
    } else if (relation_info[1] == 1) {
        return stats.distinct_source_relations[relation_info[0]];
    } else {
        return (uint32_t)-1;
    }
}

std::vector<std::string> reverse_path(std::vector<std::string> path) {
    std::vector<std::string> newPath;
    for (int i = path.size()-1; i >= 0 ; i--) {
        if (path[i].substr(1, 2) == ">")
            newPath.push_back(path[i].substr(0, 1)+(std::string)"<");
        else if (path[i].substr(1, 2) == "<")
            newPath.push_back(path[i].substr(0, 1)+(std::string)">");
        else
            newPath.push_back(path[i]);
    }
    return newPath;
}

/////
///// SimpleEstimator class
/////
SimpleEstimator::SimpleEstimator(std::shared_ptr<SimpleGraph> &g){

    // works only with SimpleGraph
    graph = g;
}

void SimpleEstimator::prepare() {
    uint32_t noLabels = graph->getNoLabels();
    uint32_t noVertices = graph->getNoVertices();

    stats = Stats(noLabels, noVertices);
    // // stats.create_stats(&graph->positions_adj, &graph->positions_adj_reverse, &graph->IA, &graph->IA_reverse);
    stats.create_stats(&graph);
}


/// Parse tree to Vector of queries
void inorderParse(PathTree *node,
                  std::vector<std::string> *query) {
    if (node == nullptr) {
        return;
    }
    inorderParse(node->left, query);

    if (node->data != "/") {
        query->push_back(node->data);
    }

    inorderParse(node->right, query);
}

std::vector<std::string> parsePathTree(PathTree *tree) {
    std::vector<std::string> query;

    if (!tree->isLeaf()) {
        inorderParse(tree, &query);
    } else {
        query.push_back(tree->data);
    }
    return query;
}

/// Sample transitive closure queries
// std::vector<std::pair<uint32_t, uint32_t>> SimpleEstimator::SampleTransitiveClosure(int T, float sample) {
//     auto se = SimpleEvaluator(graph);

//     int sampleSize = ceil(sample * graph->getNoVertices());
//     int numNewAdded = 1;

//     /// Create sample graph (TC)
//     // Use max upperbound for labels
//     auto sampleGraph = std::make_shared<SimpleGraph>(graph->getNoVertices());
//     sampleGraph->setNoLabels(sampleSize);

//     // Use max upperbound for labels
//     auto base = std::make_shared<SimpleGraph>(graph->getNoVertices());
//     base->setNoLabels(sampleSize);

//     while (numNewAdded) {
//         auto delta = se.join(sampleGraph, base);
//         numNewAdded = se.unionDistinct(sampleGraph, delta);
//     }

//     return sampleGraph;
// }

/// Sample transitive closure for 1 source or target
// TODO: Adjust after changing the graph calculation doesn't work anymore
// std::shared_ptr<SimpleGraph> SimpleEstimator::SampleTransitiveClosure(int T, int node, bool reverse) {
//     auto se = SimpleEvaluator(graph);
//     int numNewAdded = 1;

//     /// Create sample graph (TC)
//     auto sampleGraph = std::make_shared<SimpleGraph>(graph->getNoVertices());
//     sampleGraph->setNoLabels(1);

//     auto base = std::make_shared<SimpleGraph>(graph->getNoVertices());
//     base->setNoLabels(1);

//     while (numNewAdded) {
//         auto delta = se.join(sampleGraph, base);
//         numNewAdded = se.unionDistinct(sampleGraph, delta);
//     }

//     return sampleGraph;
// }

cardStat SimpleEstimator::estimate(PathQuery *q) {
    int32_t rel_type = -1; /// Current Tuple "Table"
    auto path = parsePathTree(q->path);

    /// Defaults to 1, unless we know there are no tuples.
    uint32_t noSources = 1;
    uint32_t noPaths = 1;
    uint32_t noTargets = 1;

    /// Either there are no joins (e.g. just 1 relation/table)
    /// or it's a transitive closure (TC).
    if (path.size() == 1) {
        rel_type = std::stoi(path[0].substr(0, path[0].size()-1));
        std::string relation = path[0].substr(path[0].size()-1, 1);

        if (relation == ">") { // forward relation, (s,t) such that (s, l, t)
            if (q->s == "*") {
                if (q->t =="*") { // source: *, target: *
                    noSources = stats.distinct_source_relations[rel_type];
                    noPaths = stats.total_relations[rel_type];
                    noTargets = stats.distinct_target_relations[rel_type];
                } else { // source: *, target: i
                    int t_i = std::stoi(q->t);
                    int result = stats.target_relations_count[rel_type][t_i];
                    noSources = result;
                    noPaths = result;
                    noTargets = 1;
                }
            } else {
                int s_i = std::stoi(q->s);

                if (q->t =="*") { // source: i, target: *
                    int result = stats.source_relations_count[rel_type][s_i];
                    noSources = 1;
                    noPaths = result;
                    noTargets = result;
                } else { // source: i, target: j
                    int t_i = std::stoi(q->t);
                    int result = std::min(stats.target_relations_count[rel_type][t_i],
                                          stats.source_relations_count[rel_type][s_i]);
                    noSources = result;
                    noPaths = result;
                    noTargets = result;
                }
            }
        } else if(relation == "<") { // backward relation, (s,t) such that (t, l, s)
            if (q->s == "*") {
                if (q->t =="*") { // source: *, target: *
                    noSources = stats.distinct_target_relations[rel_type];
                    noPaths = stats.total_relations[rel_type];
                    noTargets = stats.distinct_source_relations[rel_type];
                } else { // source: *, target: j
                    int t_i = std::stoi(q->t);
                    int result = stats.source_relations_count[rel_type][t_i];
                    noSources = result;
                    noPaths = result;
                    noTargets = 1;
                }
            } else {
                int s_i = std::stoi(q->s);

                if (q->t =="*") { // source: i, target: *
                    int result = stats.target_relations_count[rel_type][s_i];
                    noSources = 1;
                    noPaths = result;
                    noTargets = result;
                } else { // source: i, target: j
                    int t_i = std::stoi(q->t);
                    int result = std::min(stats.source_relations_count[rel_type][t_i],
                                          stats.target_relations_count[rel_type][s_i]);
                    noSources = result;
                    noPaths = result;
                    noTargets = result;
                }
            }
        }
        else if(relation == "+") { // Transitive Closure relation

            // The sample size for sampling on the entire graph
            float sample = 0.05;

            if (q->s == "*") { // - Source: *
                if (q->t =="*") { // - Source: *, Target: *
                    noSources = stats.distinct_source_relations[rel_type];
                    noTargets = stats.distinct_target_relations[rel_type];
                    noPaths = stats.total_relations[rel_type] + stats.multidimensional_matrix[rel_type][rel_type][0][0][0];
                } else { // - Source: *, Target: i
                    int t_i = std::stoi(q->t);
                    // auto out = SampleTransitiveClosure(rel_type, t_i, true);

                    // noSources = out->getNoDistinctEdges();
                    // noPaths = out->getNoDistinctEdges();
                    noTargets = 1;
                }
            } else {
                int s_i = std::stoi(q->s);

                if (q->t =="*") { // - Source: i, Target: *
                    // auto out = SampleTransitiveClosure(rel_type, s_i, false);

                    noSources = 1;
                    // noPaths = out->getNoDistinctEdges();
                    // noTargets = out->getNoDistinctEdges();
                } else { // - Source: i, Target: j
                }
            }
        }
    } else if(path.size() > 1) {
        /// There is atleast 1 join in the query,
        /// Cases of joins:
        /// Order doesn't matter => s = "*" and t = "*"
        /// Order right to left => s = "*" and t = 1
        /// Order left to right => s = 1 and t = "*", so reverse
        if (q->t != "*") {
            path = reverse_path(path);
        }


        if ((q->s == "*") && (q->t == "*")) { // source: *
            if (q->t == "*") { // source: *, target: *
                std::vector<uint32_t> relation_i;
                std::vector<uint32_t> relation_j;

                relation_i = get_relation_info(path[0]);
                relation_j = get_relation_info(path[1]);
                
                std::string relation = path[0].substr(path[0].size()-1, 1);

                if (relation == "+") { // forward relation, (s,t) such that (s, l, t)
                    float tuples_i;
                    float tuples_j;
                    float tuples_ix;
                    float tuples_jx;
                    float tuples;
                    float d_s;
                    float d_m;
                    float d_o;
                    float d_si;
                    float d_oi;
                    float d_sj;
                    float d_oj;
                    std::vector<uint32_t> join_stats;

                    // calc
                    float perc;

                    d_si = stats.distinct_source_relations[relation_i[0]];
                    d_oi = stats.distinct_target_relations[relation_i[0]];
                    tuples_i = stats.total_relations[relation_i[0]] + stats.multidimensional_matrix[relation_i[0]][relation_i[0]][0][0][0];
                    tuples_ix = tuples_i / stats.total_relations[relation_i[0]];                  

                    // first join
                    for (uint32_t j = 1; j < path.size(); j++) {
                        relation_j = get_relation_info(path[j]);

                        d_sj = stats.distinct_source_relations[relation_j[0]];
                        d_oj = stats.distinct_target_relations[relation_j[0]];
                        tuples_j = stats.total_relations[relation_j[0]] + stats.multidimensional_matrix[relation_j[0]][relation_j[0]][0][0][0];
                        tuples_jx = tuples_j / stats.total_relations[relation_j[0]]; 

                        join_stats = stats.multidimensional_matrix[relation_i[0]][relation_j[0]][0][0];
                        tuples = join_stats[0];
                        d_s = join_stats[1];
                        d_m = join_stats[2];
                        d_o = join_stats[3];

                        // perc = d_m / d_oi;
                        tuples_i = tuples_ix  * tuples * tuples_jx; // * perc
                        tuples_ix = tuples_i / stats.total_relations[relation_j[0]]; // std::abs(tuples_i - tuples_j);
                        d_si = d_si * (d_s / stats.distinct_source_relations[relation_i[0]]) * tuples_jx;
                        d_oi = d_oj * (d_o / stats.distinct_target_relations[relation_j[0]]);

                        relation_i = relation_j;
                    }

                    noSources = d_si;
                    noPaths = tuples_i;
                    noTargets = d_oi;
                } else {
                    // basic info
                    float in;     // l1.in
                    float T_i;
                    float d_si;
                    // uint32_t middle_i;
                    float d_oi;   // d(o, T_{r/l1})

                    float part1;

                    // multidimensional matrix
                    std::vector<uint32_t> join_stats;
                    float T_j;        // |T_{l1/l2}| -> |T_{j-1/j}|
                    float d_sj;       // d(s, T_{l1/l2})
                    float middle_j;   // l1/l2.middle
                    float d_oj;       // d(o, T_{l1/l2})

                    join_stats = stats.multidimensional_matrix[relation_i[0]][relation_j[0]][relation_i[1]][relation_j[1]];
                    T_i = join_stats[0];
                    d_si = join_stats[1];
                    d_oi = join_stats[3];

                    for (int j = 2; j < path.size(); j++) {
                        relation_i = relation_j;
                        relation_j = get_relation_info(path[j]);

                        in = get_in(relation_i);

                        join_stats = stats.multidimensional_matrix[relation_i[0]][relation_j[0]][relation_i[1]][relation_j[1]];
                        T_j = join_stats[0];
                        d_sj = join_stats[1];
                        middle_j = join_stats[2];
                        d_oj = join_stats[3];

                        // calculations
                        part1 = middle_j / in;
                        // part1 = middle_j / d_oi;
                        d_si = d_si * part1;
                        T_i = T_i * part1 * (T_j / d_sj)/4;
                        d_oi = d_oi * d_oj / in;
                    }

                    noSources = d_si;
                    noPaths = T_i;
                    noTargets = d_oi;
                }
            }
        } else {
            if (q->t == "*" || q->s == "*") { // source: i, target: *
                uint32_t source;
                // basic info
                float in;     // l1.in
                float T_i;
                float d_si;
                // uint32_t middle_i;
                float d_oi;   // d(o, T_{r/l1})

                std::vector<uint32_t> relation_i;
                std::vector<uint32_t> relation_j;
                float part1;

                relation_i = get_relation_info(path[0]);
                if ((q->t == "*"))
                    source = std::stoi(q->s);
                else
                    source = std::stoi(q->t);
                if (relation_i[1] == 0)
                    T_i = stats.source_relations_count[relation_i[0]][source];
                else
                    T_i = stats.target_relations_count[relation_i[0]][source];

                // multidimensional matrix
                std::vector<uint32_t> join_stats;
                float T_j;        // |T_{l1/l2}| -> |T_{j-1/j}|
                float d_sj;       // d(s, T_{l1/l2})
                float middle_j;   // l1/l2.middle
                float d_oj;       // d(o, T_{l1/l2})

                d_oi = T_i;

                for (int j = 1; j < path.size(); j++) {
                    relation_j = get_relation_info(path[j]);
                    in = get_in(relation_i);

                    join_stats = stats.multidimensional_matrix[relation_i[0]][relation_j[0]][relation_i[1]][relation_j[1]];
                    T_j = join_stats[0];
                    d_sj = join_stats[1];
                    middle_j = join_stats[2];
                    d_oj = join_stats[3];

                    // calculations
                    part1 = middle_j / in;
//                    d_si = d_si * part1;
                    T_i = T_i * part1 * (T_j / d_sj);
                    d_oi = d_oi * d_oj / in;


                    relation_i = relation_j;
                    T_i = (T_i+d_oi)/2;
                    d_oi = T_i;
                }

                noPaths = T_i;
                if (q->t == "*") {
                    noSources = T_i > 0;
                    noTargets = d_oi;
                }
                else {
                    noSources = d_oi;
                    noTargets = T_i > 0;
                }
            } else { // source: i, target: j
                uint32_t source;
                // basic info
                float in;     // l1.in
                float T_i;
                float d_si;
                // uint32_t middle_i;
                float d_oi;   // d(o, T_{r/l1})

                std::vector<uint32_t> relation_i;
                std::vector<uint32_t> relation_j;
                float part1;

                relation_i = get_relation_info(path[0]);
                source = std::stoi(q->t);
                if (relation_i[1] == 0)
                    T_i = stats.source_relations_count[relation_i[0]][source];
                else
                    T_i = stats.target_relations_count[relation_i[0]][source];

                // multidimensional matrix
                std::vector<uint32_t> join_stats;
                float T_j;        // |T_{l1/l2}| -> |T_{j-1/j}|
                float d_sj;       // d(s, T_{l1/l2})
                float middle_j;   // l1/l2.middle
                float d_oj;       // d(o, T_{l1/l2})

                for (int j = 1; j < path.size(); j++) {
                    d_oi = T_i;
                    relation_j = get_relation_info(path[j]);
                    in = get_in(relation_i);

                    join_stats = stats.multidimensional_matrix[relation_i[0]][relation_j[0]][relation_i[1]][relation_j[1]];
                    T_j = join_stats[0];
                    d_sj = join_stats[1];
                    middle_j = join_stats[2];
                    d_oj = join_stats[3];

                    // calculations
                    part1 = middle_j / in;
//                    d_si = d_si * part1;
                    T_i = T_i * part1 * (T_j / d_sj);
                    d_oi = d_oi * d_oj / in;


                    relation_i = relation_j;
                    T_i = (T_i+d_oi)/2;
                }

                if (d_oi > 0) {
                    noPaths = T_i/d_oi;
                    noSources = 1;
                    noTargets = 1;
                }
                else {
                    noPaths = 0;
                    noSources = 0;
                    noTargets = 0;
                }
            }
        }
    }

    return cardStat {noSources, noPaths, noTargets};
}