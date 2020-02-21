#include "SimpleGraph.h"
#include "SimpleEstimator.h"
// #include <Histogram.h>
#include <set>
#include <cmath>


Histogram::Histogram(std::string &type_of_histogram, uint32_t noLabels, 
        uint32_t noVertices, uint32_t u_depth, uint32_t u_width_size) {
    labels = noLabels;
    vertices = noVertices;
    depth = u_depth;
    width_size = u_width_size;
    histogram_type = type_of_histogram;
}

Histogram::~Histogram() {
//    TODO: Free all memory
}

void Histogram::create_histograms(std::vector<std::vector<std::pair<uint32_t, uint32_t>>> adj) {
    create_frequency_vectors(adj);
    std::cout << "Type of histogram to be created: " << histogram_type << std::endl;
    if (histogram_type == "equidepth")
        create_equidepth_histograms();
    else if (histogram_type == "equiwidth")
        create_equiwidth_histograms();
//    else if (histogram_type == "voptimal")
//         create_voptimal_histograms(adj);
}

void Histogram::create_equidepth_histograms() {
    for (int i = 0; i < labels; i++) {
        std::cout << "Relation " << i << " has size " << source_relations_count[i].size() << std::endl;
        uint32_t n = 0;
        source_buckets.push_back({});
        source_buckets[i].push_back({});
        for (int j = 0; j < source_relations_count[i].size(); j++) {
            uint32_t start = j;
            uint32_t end = j;
            uint32_t sum = 0;
            while (sum < depth) {
                if ((sum > 0) && (source_relations_count[i][j] > depth)) {
                    break;
                } else if (j == source_relations_count[i].size()) {
                    break;
                } else {
                    sum += source_relations_count[i][j];
                    end = j;
                    j++;
                }
            }
            source_buckets[i][n].push_back(start);
            source_buckets[i][n].push_back(end);
            source_buckets[i][n].push_back(sum);
            n++;
            if (j < source_relations_count[i].size()) {
                source_buckets[i].push_back({});
                j--;
            }
        }
        n = 0;
        target_buckets.push_back({});
        target_buckets[i].push_back({});
        for (int j = 0; j < target_relations_count[i].size(); j++) {
            uint32_t start = j;
            uint32_t end = j;
            uint32_t sum = 0;
            while (sum < depth) {
                if ((sum > 0) && (target_relations_count[i][j] > depth)) {
                    break;
                } else if (j == target_relations_count[i].size()) {
                    break;
                } else {
                    sum += target_relations_count[i][j];
                    end = j;
                    j++;
                }
            }
            target_buckets[i][n].push_back(start);
            target_buckets[i][n].push_back(end);
            target_buckets[i][n].push_back(sum);
            n++;
            if (j < target_relations_count[i].size()) {
                target_buckets[i].push_back({});
                j--;
            }
        }
    }
}

void Histogram::create_equiwidth_histograms() {
    for (int i = 0; i < labels; i++) {
        std::cout << "Relation " << i << " has size " << source_relations_count[i].size() << std::endl;
        uint32_t n = 0;
        source_buckets.push_back({});
        source_buckets[i].push_back({});
        for (int j = 0; j < source_relations_count[i].size(); j++) {
            uint32_t count_it = 0;
            uint32_t start = j;
            uint32_t sum = 0;
            while (count_it < width_size) {
                if (j == source_relations_count[i].size()) {
                    break;
                } else {
                    sum += source_relations_count[i][j];
                    j++;
                    count_it++;
                }
            }
            source_buckets[i][n].push_back(start);
            source_buckets[i][n].push_back(j - 1);
            source_buckets[i][n].push_back(sum);
            n++;
            if (j < source_relations_count[i].size()) {
                source_buckets[i].push_back({});
                j--;
            }
        }
        n = 0;
        target_buckets.push_back({});
        target_buckets[i].push_back({});
        for (int j = 0; j < target_relations_count[i].size(); j++) {
            uint32_t count_it = 0;
            uint32_t start = j;
            uint32_t sum = 0;
            while (count_it < width_size) {
                count_it++;
                if (j == target_relations_count[i].size()) {
                    break;
                } else {
                    sum += target_relations_count[i][j];
                    j++;
                }
            }
            target_buckets[i][n].push_back(start);
            target_buckets[i][n].push_back(j - 1);
            target_buckets[i][n].push_back(sum);
            n++;
            if (j < target_relations_count[i].size()) {
                target_buckets[i].push_back({});
                j--;
            }
        }
    }
}

void Histogram::create_frequency_vectors(std::vector<std::vector<std::pair<uint32_t, uint32_t>>> adj) {
    for (int i = 0; i < labels; i++) {
        relations.push_back({});
        source_relations_count.push_back({});
        target_relations_count.push_back({});
        for (int k = 0; k < vertices; k++) {
            source_relations_count[i].push_back({0});
            target_relations_count[i].push_back({0});
        }
    }

    for (uint32_t i = 0; i < adj.size(); i++){
        for (uint32_t j = 0; j < adj[i].size() ; j++) {
            uint32_t rel_type = adj[i][j].first;
            uint32_t rel_target = adj[i][j].second;
            source_relations_count[rel_type][i]++;
            target_relations_count[rel_type][rel_target]++;
            relations[rel_type].push_back(std::make_pair(i, rel_target));
        }
    }

//    Print size of each relation
//    for (int i = 0; i < labels; i++) {
//        uint32_t relation_sum = 0;
//        for (int k = 0; k < vertices; k++) {
//            relation_sum += source_relations_count[i][k];
//            std::cout << source_relations_count[i][k] << std::endl;
//        }
//        std::cout << "Relation " << i << ": " << relation_sum << std::endl;
//    }
}

void Histogram::print_histogram(std::string query_var, uint32_t relation) {
    std::cout << "Histogram " << histogram_type << " for " << query_var << " relation " << relation << std::endl;
    if (query_var == "source") {
        for (int i = 0; i < source_buckets[relation].size(); i++) {
            std::cout << source_buckets[relation][i][0] << "\t" << source_buckets[relation][i][1] << "\t"
                      << source_buckets[relation][i][2] << std::endl;
        }
    }
    else if (query_var == "target") {
        for (int i = 0; i < target_buckets[relation].size(); i++) {
            std::cout << target_buckets[relation][i][0] << "\t" << target_buckets[relation][i][1] << "\t"
                      << target_buckets[relation][i][2] << std::endl;
        }
    }
    std::cout << std::endl;

}

uint32_t Histogram::get_query_results(uint32_t nodeID, std::string query_var, uint32_t relation) {
    if (nodeID > vertices)
        return -1;
    int i = 0;
    if (query_var == "source") {
        int noBuckets = source_buckets[relation].size();
        while (nodeID > source_buckets[relation][i][1]) {
            i++;
            if (i == noBuckets)
                return -1;
        }
        return source_buckets[relation][i][2];
    }
    else if (query_var == "target") {
        int noBuckets = target_buckets[relation].size();
        while (nodeID > target_buckets[relation][i][1]) {
            i++;
            if (i == noBuckets)
                return -1;
        }
        return target_buckets[relation][i][2];
    }
    else
        return -1;
}


SimpleEstimator::SimpleEstimator(std::shared_ptr<SimpleGraph> &g){

    // works only with SimpleGraph
    graph = g;
}

void SimpleEstimator::prepare() {

    // Preparation
    // TODO: Remove for evaluation
    std::cout << "No Edges: " << graph->getNoEdges() << std::endl;
    std::cout << "No Labels: "  << graph->getNoLabels() << std::endl;
    std::cout << "No Vertices: "  << graph->getNoVertices() << std::endl;
    std::cout << "No Distinct Edges: "  << graph->getNoDistinctEdges() << std::endl;

    int noLabels = graph->getNoLabels();
    int noVertices = graph->getNoVertices();
    int noEdges = graph->getNoEdges();

    // Sample tuple sizes
    // TODO: Find good ratio, sample 5% of the vertices
    int samples = ceil(0.05 * noVertices);

    std::vector<int> sampleCount(noLabels, 0);
    for(int i = 0; i < samples; i++) {
        int index = rand() % noVertices;

        if (graph->adj[index].size() > 0) {
            int adjIndex = rand() % graph->adj[index].size();
            int ct = graph->adj[index][adjIndex].first;
            sampleCount[ct] = sampleCount[ct] + 1;
        } else { // No relation edge found,
            i--;
        }
    }
    sampleVertices = sampleCount;


    std::cout << "tuple 0: " << 20 * sampleCount[0]
        << "\ntuple 1: " << 20 * sampleCount[1] 
        << "\ntuple 2: " << 20 * sampleCount[2] 
        << "\ntuple 3: " << 20 * sampleCount[3] 
        << "\nCount: " << 20 * (sampleCount[0] + sampleCount[1] 
        + sampleCount[2] + sampleCount[3]) << std::endl;

    // Creation histograms
    std::string histogram_type = "equiwidth";
    uint32_t u_depth = noEdges / 200;
    uint32_t u_width_size = 250;
    Histogram histogram = Histogram(histogram_type, noLabels, noVertices, u_depth, u_width_size);
    histogram.create_histograms(graph->adj);
    histogram.print_histogram("source", 0);
    std::cout << histogram.get_query_results(985, "source", 0) << std::endl;
}


// TODO: Needs some kind of Histogram to work properly, V(R, A)
int DistinctValuesFor(int relation, std::string attribute) {
    // Default is 10
    return 10;
}


cardStat SimpleEstimator::estimate(PathQuery *q) {

    // TODO: Change exact indications to approximations
    std::set<int> sources = {};
    std::set<int> targets = {};
    std::vector<std::pair<uint32_t,uint32_t>> results = {};

    std::string relation_direction = q->path->data.substr(q->path->data.size()-1,1);
    std::string relation_type = q->path->data.substr(0,q->path->data.size()-1);
    std::string sourceVertex;
    std::string targetVertex;

    if (relation_direction == ">") {
        sourceVertex = q->s;
        targetVertex = q->t;
    }

    else if (relation_direction == "<") {
        sourceVertex = q->t;
        targetVertex = q->s;
    }

    if (sourceVertex != "*") {
        uint32_t int_source = std::stoul(sourceVertex,0);

        if (targetVertex != "*") {
            uint32_t int_target = std::stoul(targetVertex,0);
            for (uint32_t j = 0; j < graph->adj.at(int_source).size() ; j++){
                if ((graph->adj.at(int_source).at(j).first == std::stoul(relation_type,0)) && (graph->adj.at(int_source).at(j).second == int_target)) {
                    results.push_back(std::make_pair(int_source, int_target));
                    sources.insert(int_source);
                    targets.insert(int_target);
                }
            }
        }

        else {
            for (uint32_t j = 0; j < graph->adj.at(int_source).size() ; j++){
                if (graph->adj.at(int_source).at(j).first == std::stoul(relation_type,0)) {
                    results.push_back(std::make_pair(int_source, graph->adj.at(int_source).at(j).second));
                    sources.insert(int_source);
                    targets.insert(graph->adj.at(int_source).at(j).second);
                }
            }
        }
    }
    else {

        if (targetVertex != "*") {
            uint32_t int_target = std::stoul(targetVertex,0);
            for (uint32_t i = 0; i < graph->adj.size(); i++) {
                for (uint32_t j = 0; j < graph->adj.at(i).size() ; j++){
                    if ((graph->adj.at(i).at(j).first == std::stoul(relation_type,0)) && (graph->adj.at(i).at(j).second == int_target)) {
                        results.push_back(std::make_pair(i, int_target));
                        sources.insert(i);
                        targets.insert(int_target);
                    }
                }
            }
        }
        else {
            for (uint32_t i = 0; i < graph->adj.size(); i++) {
                for (uint32_t j = 0; j < graph->adj.at(i).size() ; j++){
                    if (graph->adj.at(i).at(j).first == std::stoul(relation_type,0)) {
                        results.push_back(std::make_pair(i, graph->adj.at(i).at(j).second));
                        sources.insert(i);
                        targets.insert(graph->adj.at(i).at(j).second);
                    }
                }
            }
        }
    }

    uint32_t noSources = sources.size();
    uint32_t noPaths = results.size();
    uint32_t noTargets = targets.size();

    return cardStat {noSources, noPaths, noTargets};
}
