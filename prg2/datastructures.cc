// Datastructures.cc

#include "datastructures.hh"

#include <random>

#include <cmath>
// Added by students
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <QDebug>
#include <vector>
#include <queue>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures()
{
    isBrightnessSorted = false;
    isNameSorted = false;
    isBrightestValid = false;
    isDimmestValid = false;

    canAllFibresBeTrusted = false;
    middle_fibre = all_fibres_.begin();
    allFibreSize = 0;

    canAllXpointsBeTrusted = false;
    middle_xpoint = all_xpoints_.begin();
    allXpointSize = 0;
}

Datastructures::~Datastructures()
{

}

int Datastructures::beacon_count()
{
    std::size_t size = id_data.size();
    return size;
}

void Datastructures::clear_beacons()
{
    id_data.clear();
    isBrightnessSorted = false;
    isNameSorted = false;
    isBrightestValid = false;
    isDimmestValid = false;
}

std::vector<BeaconID> Datastructures::all_beacons()
{
    std::vector<BeaconID> all_beacons;
    for (auto beacon : id_data) {
        all_beacons.push_back(beacon.first);
    }
    return all_beacons;
}

int Datastructures::get_brightness(Color color) {
    int red = color.r;
    int green = color.g;
    int blue = color.b;
    int brightness = (3*red) + (6*green) + blue;
    return brightness;
}

bool Datastructures::add_beacon(BeaconID id, const std::string& name, Coord xy, Color color)
{
    if (id_data.find(id) == id_data.end()) {
        // Creat new Beacon object, assign new pointer and add to data structure
        auto new_beacon = std::make_shared<Beacon>(Beacon{id, name, color, xy, {}, "", {}, false});
        id_data.insert({id, new_beacon});
        // Get the new beacon's brightness, check if it is the new max/min bright beacon,
        // if yes, we'll have to sort the beacons based on brightness again if we want to
        // get the max/min bright.
        int brightness = get_brightness(color);
        if (brightness > max_bright.second) {
            isBrightestValid = false;
        } else if (brightness < min_bright.second) {
            isDimmestValid = false;
        }
        isBrightnessSorted = false;
        isNameSorted = false;
        return true;
    } else {
        return false;
    }
}

std::string Datastructures::get_name(BeaconID id)
{
    if (id_data.find(id) == id_data.end()) {
        return NO_NAME;
    } else {
        std::string name = id_data.at(id)->name;
        return name;
    }
}

Coord Datastructures::get_coordinates(BeaconID id)
{
    if (id_data.find(id) == id_data.end()) {
        return NO_COORD;
    } else {
        return id_data.at(id)->coord;
    }

}

Color Datastructures::get_color(BeaconID id)
{
    if (id_data.find(id) == id_data.end()) {
        return NO_COLOR;
    } else {
        return id_data.at(id)->color;
    }
}

// Function to sort beacons' ID-name pairs by name
bool compare_beacon_names(const std::pair<BeaconID, std::string> p1, const std::pair<BeaconID, std::string> p2)
{
    return p1.second < p2.second;
}

std::vector<BeaconID> Datastructures::beacons_alphabetically()
{
    if (id_data.size() != 0) {
        if (isNameSorted == false) {
            beacon_alpha.clear();
            // Vector of ID and name pairs, which will be sorted by brightness
            std::vector<std::pair<std::string, std::string>> id_names;
            for(auto beacon : id_data) {
                BeaconID id = beacon.first;
                std::string name = beacon.second->name;
                std::pair<BeaconID, std::string> id_name (id, name);
                id_names.push_back(id_name);
            }
            std::sort(id_names.begin(), id_names.end(), &compare_beacon_names);
            // Add IDs of the sorted beacons' pairs to the to-be-returned vector
            for (auto pair : id_names) {
                beacon_alpha.push_back(pair.first);
            }
            isNameSorted = true;
            return beacon_alpha;
        } else {
            return beacon_alpha;
        }
    } else {
        return {};
    }
}

// Function to sort beacons' ID-brightness pairs by brightness
bool compare_beacon_brightness(const std::pair<BeaconID, int> p1, const std::pair<BeaconID, int> p2)
{
    return p1.second < p2.second;
}

std::vector<BeaconID> Datastructures::beacons_brightness_increasing()
{
    if (id_data.size() != 0) {
        if (isBrightnessSorted == false) {
            beacon_brightness.clear();
            // Vector of ID and brightness pairs, which will be sorted by brightness
            std::vector<std::pair<BeaconID, int>> id_brightness;
            for(auto beacon : id_data) {
                // brightness = 3*red + 6*green + blue
                Color color_info = beacon.second->color;
                int brightness = get_brightness(color_info);
                std::pair<BeaconID, int> id_bright_pair (beacon.first, brightness);
                id_brightness.push_back(id_bright_pair);
            }
            std::sort(id_brightness.begin(), id_brightness.end(), &compare_beacon_brightness);
            // Add IDs from the vector of pairs to the to-be-returned vector
            for (auto pair : id_brightness) {
                beacon_brightness.push_back(pair.first);
            }
            isBrightnessSorted = true;
            // Obtain the min and max bright beacon IDs if the current values are false
            if (isBrightestValid == false) {
                max_bright = id_brightness.back();
                isBrightestValid = true;
            }
            if (isDimmestValid == false) {
                min_bright = id_brightness.front();
                isDimmestValid = true;
            }
            return beacon_brightness;
        } else {
            return beacon_brightness;
        }
    } else {
        return {};
    }
}

BeaconID Datastructures::min_brightness()
{
    if (isDimmestValid == false) {
        std::vector<BeaconID> beacon_brightness = beacons_brightness_increasing();
        if (beacon_brightness.size() != 0) {
            return min_bright.first;
        } else {
            return NO_ID;
        }
    } else {
        return min_bright.first;
    }
}

BeaconID Datastructures::max_brightness()
{
    if (isBrightestValid == false) {
        std::vector<BeaconID> beacon_brightness = beacons_brightness_increasing();
        if (beacon_brightness.size() != 0) {
            return max_bright.first;
        } else {
            return NO_ID;
        }
    } else {
        return max_bright.first;
    }
}

std::vector<BeaconID> Datastructures::find_beacons(std::string const& name)
{
    std::vector<BeaconID> name_beacons = {};
    for (auto beacon : id_data) {
        if (beacon.second->name == name) {
            name_beacons.push_back(beacon.first);
        }
    }
    std::sort(name_beacons.begin(), name_beacons.end());
    return name_beacons;
}

bool Datastructures::change_beacon_name(BeaconID id, const std::string& newname)
{
    if (id_data.find(id) != id_data.end()) {
        id_data.at(id)->name = newname;
        isNameSorted = false;
        return true;
    } else {
        return false;
    }
}

bool Datastructures::change_beacon_color(BeaconID id, Color newcolor)
{
    if (id_data.find(id) != id_data.end()) {
        id_data.at(id)->color = newcolor;
        isBrightnessSorted = false;
        int brightness = get_brightness(newcolor);
        if (brightness > max_bright.second) {
            isBrightestValid = false;
        } else if (brightness < min_bright.second) {
            isDimmestValid = false;
        }
        return true;
    } else {
        return false;
    }
}

bool Datastructures::add_lightbeam(BeaconID sourceid, BeaconID targetid)
{
    if (id_data.find(sourceid) == id_data.end() || id_data.find(targetid) == id_data.end()) {
        return false;
    } else {
        std::shared_ptr<Beacon> source = id_data.at(sourceid);
        std::shared_ptr<Beacon> target = id_data.at(targetid);
        if (source->target_ != ""){
            return false;
        } else {
            target->sources_.push_back(sourceid);
            target->isSourcesSorted = false;
            target->sources_nodes_.push_back(source);
            source->target_ = targetid;
            return true;
        }
    }
}

std::vector<BeaconID> Datastructures::get_lightsources(BeaconID id)
{
    if (id_data.find(id) != id_data.end()) {
        std::shared_ptr<Beacon> target = id_data.at(id);
        if (target->isSourcesSorted == true) {
            return target->sources_;
        } else {
            std::sort(target->sources_.begin(), target->sources_.end());
            target->isSourcesSorted = true;
            return target->sources_;
        }
    } else {
        return {{NO_ID}};
    }
}

std::vector<BeaconID> Datastructures::path_outbeam(BeaconID id)
{
    std::vector<BeaconID> targets;
    targets.push_back(id);
    if (id_data.at(id)->target_ == "") {
        return targets;
    } else {
        while (id_data.at(id)->target_ != "") {
            BeaconID target_id = id_data.at(id)->target_;
            targets.push_back(target_id);
            id = target_id;
        }
    return targets;
    }
}

bool Datastructures::remove_beacon(BeaconID id)
{
    if (id_data.find(id) == id_data.end()) {
        return false;
    } else {
        isBrightnessSorted = false;
        isNameSorted = false;
        if (id == max_bright.first) {
            isBrightestValid = false;
        } else if (id == min_bright.first) {
            isDimmestValid = false;
        }

        std::shared_ptr<Beacon> this_beacon = id_data.at(id);
        // Remove this beacon as another beacon's source
        if (this_beacon->target_ != "") {
            BeaconID its_target = this_beacon->target_;
            std::shared_ptr<Beacon> target = id_data.at(its_target);
            // Find the id and remove it
            auto removing_source = std::find(target->sources_.begin(), target->sources_.end(), id);
            if (removing_source != target->sources_.end()){
                target->sources_.erase(std::remove(target->sources_.begin(), target->sources_.end(), id));
                target->isSourcesSorted = false;
            }
            // Find the pointer and remove it
            auto removing_source_ptr = std::find(target->sources_nodes_.begin(), target->sources_nodes_.end(), this_beacon);
            if (removing_source_ptr != target->sources_nodes_.end()){
                target->sources_nodes_.erase(std::remove(target->sources_nodes_.begin(), target->sources_nodes_.end(), this_beacon));
            }
        }

        // Remove this beacon as other beacons' targets
        if (this_beacon->sources_.size()!=0) {
            std::vector<BeaconID> erasing_sources = this_beacon->sources_;
            for (BeaconID erasing_id : erasing_sources) {
                id_data.at(erasing_id)->target_ = "";
            }
        }

        // Remove beacon completely
        id_data.erase(id);
        return true;
    }
}

// Find height of tree
unsigned long Datastructures::height (std::shared_ptr<Beacon> beacon) {
    if (beacon == nullptr){
        return 0;
    } else {
        unsigned long h = 0;
        for (std::shared_ptr<Beacon> source : beacon->sources_nodes_) {
            h = std::max(h, height(source));
        }
        return h+1;
    }
}
// Find the longest inbeam chain and 'return' the result to the vector declared in path_inbeam_longest
void Datastructures::find_longest (std::shared_ptr<Beacon> beacon, const unsigned long h,
                                   std::vector<BeaconID>& inbeam, BeaconID id, unsigned long index)
{
    if (beacon->sources_.size() == 0) {
        if (index == h) {
            // If this beacon is the leaf (or one of the leaves) which depth
            // equals height, add it to the vector. Therefore, the size of this
            // vector will be 0 if this beacon is not the desired leaf.
            inbeam.push_back(id);
        }
    } else {
        for (std::shared_ptr<Beacon> source : beacon->sources_nodes_) {
            BeaconID this_id = source->id;
            find_longest(source, h, inbeam, this_id, index+1);
        }
        // If the amount of beacons added equal the height - current index -> this node
        // belongs to this path -> add to vector
        if (inbeam.size() == h-index) {
            inbeam.push_back(id);
        }
        // If we got back to the root, it means we've obtained the complete path
        if (inbeam.size() == h) {
            return;
        }
    }
}

std::vector<BeaconID> Datastructures::path_inbeam_longest(BeaconID id)
{
    if (id_data.find(id) == id_data.end()) {
        return {{NO_ID}};
    } else {
        std::shared_ptr<Beacon> head = id_data.at(id);
        unsigned long h = height(head);
        std::vector<BeaconID> inbeam;
        inbeam.reserve(h);
        find_longest(head, h, inbeam, id, 1);
        return inbeam;
    }
}

// Private func defined by student
Color Datastructures::average (std::shared_ptr<Beacon> beacon)
{
    if(beacon->sources_.size() == 0) {
        return beacon->color;
    } else {
        int avg_r;
        int avg_g;
        int avg_b;
        double sum_r = beacon->color.r;
        double sum_g = beacon->color.g;
        double sum_b = beacon->color.b;

        Color avg;
        for (std::shared_ptr<Beacon> source : beacon->sources_nodes_) {
            Color source_color = average(source);
            sum_r += source_color.r;
            sum_g += source_color.g;
            sum_b += source_color.b;
        }
        avg_r = floor(sum_r/(beacon->sources_.size() + 1));
        avg_g = floor(sum_g/(beacon->sources_.size() + 1));
        avg_b = floor(sum_b/(beacon->sources_.size() + 1));
        avg = {avg_r, avg_g, avg_b};
        return avg;
    }
}

Color Datastructures::total_color(BeaconID id)
{
    if (id_data.find(id) != id_data.end()) {
        std::shared_ptr<Beacon> head = id_data.at(id);
        return average(head);
    } else {
        return NO_COLOR;
    }
}

std::vector<Coord> Datastructures::all_xpoints()
{
    if (canAllXpointsBeTrusted){
        return all_xpoints_;
    } else {
        // If all_xpoints_ is not sorted, find the iterator separating the sorted and
        // unsorted elements
        if (all_xpoints_.size() > 0){
            middle_xpoint = all_xpoints_.begin() + allXpointSize;
        } else {
            middle_xpoint = all_xpoints_.begin();
            allXpointSize = 0;
            return {};
        }
        // Sort the unsorted part
        std::sort(middle_xpoint, all_xpoints_.end());
        // If there was no sorted elements before, we don't have to merge since we sorted
        // the whole vector
        if (middle_xpoint != all_xpoints_.begin()){
            std::inplace_merge(all_xpoints_.begin(), middle_xpoint, all_xpoints_.end());
        }
        canAllXpointsBeTrusted = true;
        allXpointSize = all_xpoints_.size(); // update new size
        return all_xpoints_;
    }
}

bool Datastructures::add_fibre(Coord xpoint1, Coord xpoint2, Cost cost)
{
    if (xpoint1 == xpoint2){
        return false;
    } else {
        bool bothFound = false;
        bool firstFound = false;
        bool secondFound = false;
        // The smaller coord comes before in the coords pair
        if (xpoint2 < xpoint1){
            std::swap(xpoint1, xpoint2);
        }

        auto it1 = all_vertices_.find(xpoint1);
        auto it2 = all_vertices_.find(xpoint2);

        if (it1 != all_vertices_.end() && it2 != all_vertices_.end()) {
            //qDebug() << "Both found";
            bothFound = true;
        } else if (it1 != all_vertices_.end() && it2 == all_vertices_.end()){
            //qDebug() << "One found";
            firstFound = true;
        } else if (it2 != all_vertices_.end() && it1 == all_vertices_.end()){
            //qDebug() << "Two found";
            secondFound = true;
        }

        FibrePtr new_fibre;

        if (bothFound) {
            if (it1->second->fibres_.find(xpoint2) != it1->second->fibres_.end()){
                //qDebug() << "Fibre already existed";
                return false;
            } else {
                //qDebug() << "Xpoints both exist but no fibre between";
                new_fibre = std::make_shared<Fibre>(Fibre{cost, it1->second, it2->second});
                it1->second->fibres_.insert(std::make_pair(xpoint2, new_fibre));
                it2->second->fibres_.insert(std::make_pair(xpoint1, new_fibre));
            }

        } else if (firstFound && !secondFound) {
            XpointPtr new_xpoint2 = std::make_shared<Xpoint>(Xpoint{xpoint2, {}, false, dummy.begin(), 0, {}, -1, -1, nullptr, 0});
            new_xpoint2->middle_fibre_ = new_xpoint2->connecting_xpoints.begin();

            all_xpoints_.push_back(xpoint2);
            // Prevent unnecessary sorting
            if (all_xpoints_.back() < xpoint2 && canAllFibresBeTrusted){
                allXpointSize += 1;
            } else {
                canAllFibresBeTrusted = false;
            }

            all_vertices_.insert(std::make_pair(xpoint2, new_xpoint2));

            new_fibre = std::make_shared<Fibre>(Fibre{cost, it1->second, new_xpoint2});
            it1->second->fibres_.insert(std::make_pair(xpoint2, new_fibre));
            all_vertices_.at(xpoint2)->fibres_.insert(std::make_pair(xpoint1, new_fibre));

        } else if (!firstFound && secondFound) {
            XpointPtr new_xpoint1 = std::make_shared<Xpoint>(Xpoint{xpoint1, {}, false, dummy.begin(), 0, {}, -1, -1, nullptr, 0});
            new_xpoint1->middle_fibre_ = new_xpoint1->connecting_xpoints.begin();

            all_xpoints_.push_back(xpoint1);
            // Prevent unnecessary sorting
            if (all_xpoints_.back() < xpoint1 && canAllFibresBeTrusted){
                allXpointSize += 1;
            } else {
                canAllFibresBeTrusted = false;
            }

            all_vertices_.insert(std::make_pair(xpoint1, new_xpoint1));

            new_fibre = std::make_shared<Fibre>(Fibre{cost, new_xpoint1, it2->second});
            all_vertices_.at(xpoint1)->fibres_.insert(std::make_pair(xpoint2, new_fibre));
            it2->second->fibres_.insert(std::make_pair(xpoint1, new_fibre));

        } else {
            XpointPtr new_xpoint1 = std::make_shared<Xpoint>(Xpoint{xpoint1, {}, false, dummy.begin(), 0, {}, -1, -1, nullptr, 0});
            new_xpoint1->middle_fibre_ = new_xpoint1->connecting_xpoints.begin();
            XpointPtr new_xpoint2 = std::make_shared<Xpoint>(Xpoint{xpoint2, {}, false, dummy.begin(), 0, {}, -1, -1, nullptr, 0});
            new_xpoint2->middle_fibre_ = new_xpoint2->connecting_xpoints.begin();

            all_xpoints_.push_back(xpoint1);
            all_xpoints_.push_back(xpoint2);
            // Prevent unnecessary sorting
            if (all_xpoints_.back() < xpoint1 && canAllFibresBeTrusted){ // xpoint2 always > xpoint1
                allXpointSize += 2;
            } else {
                canAllFibresBeTrusted = false;
            }

            all_vertices_.insert(std::make_pair(xpoint1, new_xpoint1));
            all_vertices_.insert(std::make_pair(xpoint2, new_xpoint2));

            new_fibre = std::make_shared<Fibre>(Fibre{cost, new_xpoint1, new_xpoint2});
            all_vertices_.at(xpoint1)->fibres_.insert(std::make_pair(xpoint2, new_fibre));
            all_vertices_.at(xpoint2)->fibres_.insert(std::make_pair(xpoint1, new_fibre));
        }

        // Add info about coords and cost of the neighbor xpoint to the other's struct
        auto vertex_1 = all_vertices_.at(xpoint1);
        auto vertex_2 = all_vertices_.at(xpoint2);
        vertex_1->connecting_xpoints.push_back(std::make_pair(xpoint2, cost));
        vertex_2->connecting_xpoints.push_back(std::make_pair(xpoint1, cost));

        // Prevent unnecessary sorting in both xpoints' structs' connecting_xpoints
        // if the newly added is the biggest
        if (vertex_1->isFibresSorted){
            Coord coord_biggest_1 = vertex_1->connecting_xpoints.back().first;
            if (coord_biggest_1 < xpoint2){
                vertex_1->connecting_xpoints_size += 1;
            } else {
                vertex_1->isFibresSorted = false;
            }
        }
        if (vertex_2->isFibresSorted){
            Coord coord_biggest_2 = vertex_2->connecting_xpoints.back().first;
            if (coord_biggest_2 < xpoint1){
                vertex_2->connecting_xpoints_size += 1;
            } else {
                vertex_2->isFibresSorted = false;
            }
        }

        auto new_fibre_pair = std::make_pair(xpoint1, xpoint2);
        all_fibres_.push_back(new_fibre_pair);
        //qDebug() << "Added " << new_fibre_pair.first.x << "-" << new_fibre_pair.first.y << " and " << new_fibre_pair.second.x << "-" << new_fibre_pair.second.y;
        // Prevent unnecessary sorting in all_fibres_ if the newly added is the biggest
        if (new_fibre_pair > all_fibres_.back() && canAllFibresBeTrusted){
            allFibreSize += 1;
        } else {
            canAllFibresBeTrusted = false;
        }

        return true;
    }
}
std::vector<std::pair<Coord, Cost> > Datastructures::get_fibres_from(Coord xpoint)
{
    if (all_vertices_.find(xpoint) != all_vertices_.end()){
        // Find pointer to xpoint struct from all_vertices_ map
        auto this_xpoint = all_vertices_.at(xpoint);
        // If the fibres of this struct is sorted, return them, otherwise sort them then return
        if (this_xpoint->isFibresSorted){
            return this_xpoint->connecting_xpoints;
        } else {
            // Find iterator between sorted and unsorted elements
            if (this_xpoint->connecting_xpoints.size() > 0){
                this_xpoint->middle_fibre_ = this_xpoint->connecting_xpoints.begin() + this_xpoint->connecting_xpoints_size;
            } else {
                this_xpoint->middle_fibre_ = this_xpoint->connecting_xpoints.begin();
                this_xpoint->connecting_xpoints_size = 0;
                return {};
            }
            // Sort the unsorted part
            std::sort(this_xpoint->middle_fibre_, this_xpoint->connecting_xpoints.end());
            // Merge with the already sorted part if it exists
            if (this_xpoint->middle_fibre_ != this_xpoint->connecting_xpoints.begin())
                std::inplace_merge(this_xpoint->connecting_xpoints.begin(), this_xpoint->middle_fibre_,
                                   this_xpoint->connecting_xpoints.end());

            this_xpoint->isFibresSorted = true;
            this_xpoint->connecting_xpoints_size = this_xpoint->connecting_xpoints.size(); // update size
            return this_xpoint->connecting_xpoints;
        }
    } else {
        return {};
    }
}

std::vector<std::pair<Coord, Coord> > Datastructures::all_fibres()
{
    if (canAllFibresBeTrusted){
        return all_fibres_;
    } else {
        if (all_fibres_.size() > 0){
            middle_fibre = all_fibres_.begin() + allFibreSize;
        } else {
            middle_fibre = all_fibres_.begin();
            allFibreSize = 0;
            return {};
        }
        std::sort(middle_fibre, all_fibres_.end());
        if (middle_fibre != all_fibres_.begin()){
            std::inplace_merge(all_fibres_.begin(), middle_fibre, all_fibres_.end());
        }
        canAllFibresBeTrusted = true;
        allFibreSize = all_fibres_.size();
        return all_fibres_;
    }
}

bool Datastructures::remove_fibre(Coord xpoint1, Coord xpoint2)
{
    if (xpoint2 < xpoint1){
        std::swap(xpoint1, xpoint2); // xpoint 1 always < xpoint2
    }

    // Check if xpoints given exist
    if (all_vertices_.find(xpoint1) == all_vertices_.end()){
        return false;
    }
    if (all_vertices_.find(xpoint2) == all_vertices_.end()){
        return false;
    }

    auto xpoint_1 = all_vertices_.at(xpoint1);
    auto from1to2 = std::find_if(xpoint_1->connecting_xpoints.begin(), xpoint_1->connecting_xpoints.end(),
                        [&xpoint2](const std::pair<Coord, Cost>& p) {return p.first == xpoint2;});

    if (from1to2 == xpoint_1->connecting_xpoints.end()){
        return false;
    } else {
        // Remove in all_fibres_;
        auto x1x2 = std::make_pair(xpoint1, xpoint2);
        std::vector<FibrePair>::iterator find_12;
        find_12 = std::find(all_fibres_.begin(), all_fibres_.end(), x1x2);

        if(find_12 != all_fibres_.end()) {
            // If vector is not sorted, remove in constant time by swapping with the
            // last element the pop it, otherwise find then erase.
            if (!canAllFibresBeTrusted){
                if (find_12 - all_fibres_.begin() <= allFibreSize) {
                    allFibreSize = find_12 - all_fibres_.begin();
                }
                std::iter_swap(find_12, all_fibres_.end() - 1);
                all_fibres_.pop_back();
            } else {
                if (find_12 - all_fibres_.begin() <= allFibreSize){
                    allFibreSize -= 1;
                }
                all_fibres_.erase(find_12);
            }
        }

        // Remove in connecting_xpoints vector in structs of both xpoints
        auto xpoint_2 = all_vertices_.at(xpoint2);
        auto from2to1 = std::find_if(xpoint_2->connecting_xpoints.begin(), xpoint_2->connecting_xpoints.end(),
                            [&xpoint1](const std::pair<Coord, Cost>& p) {return p.first == xpoint1;});
        if (xpoint_1->isFibresSorted){
            if (from1to2 - xpoint_1->connecting_xpoints.begin() <= xpoint_1->connecting_xpoints_size){
                xpoint_1->connecting_xpoints_size -= 1;
            }
            xpoint_1->connecting_xpoints.erase(from1to2);
        } else {
            if (from1to2 - xpoint_1->connecting_xpoints.begin() <= xpoint_1->connecting_xpoints_size) {
                xpoint_1->connecting_xpoints_size = from1to2 - xpoint_1->connecting_xpoints.begin();
            }
            std::iter_swap(from1to2, xpoint_1->connecting_xpoints.end() - 1);
            xpoint_1->connecting_xpoints.pop_back();
        }

        if (xpoint_2->isFibresSorted){
            if (from2to1 - xpoint_2->connecting_xpoints.begin() <= xpoint_2->connecting_xpoints_size){
                xpoint_2->connecting_xpoints_size -= 1;
            }
            xpoint_2->connecting_xpoints.erase(from2to1);
        } else {
            if (from2to1 - xpoint_2->connecting_xpoints.begin() <= xpoint_2->connecting_xpoints_size) {
                xpoint_2->connecting_xpoints_size = from2to1 - xpoint_2->connecting_xpoints.begin();
            }
            std::iter_swap(from2to1, xpoint_2->connecting_xpoints.end() - 1);
            xpoint_2->connecting_xpoints.pop_back();
        }

        // Remove in fibres_ unordered_map in struct Xpoint
        xpoint_1->fibres_.erase(xpoint_1->fibres_.find(xpoint2));
        xpoint_2->fibres_.erase(xpoint_2->fibres_.find(xpoint1));

        // If xpoint does not have any fibre, delete the xpoint
        if (xpoint_1->fibres_.size() == 0){
            all_vertices_.erase(all_vertices_.find(xpoint1));
            auto erasing_x1 = std::find(all_xpoints_.begin(), all_xpoints_.end(), xpoint1);

            if (!canAllXpointsBeTrusted){
                if (erasing_x1 - all_xpoints_.begin() <= allXpointSize) {
                    allXpointSize = erasing_x1 - all_xpoints_.begin();
                }
                std::iter_swap(erasing_x1, all_xpoints_.end() - 1);
                all_xpoints_.pop_back();

            } else {
                if (erasing_x1 - all_xpoints_.begin() <= allXpointSize) {
                    allXpointSize -= 1;
                }
                all_xpoints_.erase(erasing_x1);
            }
        }

        if (xpoint_2->fibres_.size() == 0){
            all_vertices_.erase(all_vertices_.find(xpoint2));
            auto erasing_x2 = std::find(all_xpoints_.begin(), all_xpoints_.end(), xpoint2);

            if (!canAllXpointsBeTrusted){
                if (erasing_x2 - all_xpoints_.begin() <= allXpointSize) {
                    allXpointSize = erasing_x2 - all_xpoints_.begin();
                }
                std::iter_swap(erasing_x2, all_xpoints_.end() - 1);
                all_xpoints_.pop_back();
            } else {
                if (erasing_x2 - all_xpoints_.begin() <= allXpointSize) {
                    allXpointSize -= 1;
                }
                all_xpoints_.erase(erasing_x2);
            }
        }
    }
    return true;
}

void Datastructures::clear_fibres()
{
    // Clear all 3 "main" data structures, which also clear all pointers to Xpoints
    // and Fibres, reset flags, iterators and size values

    all_fibres_.clear();
    canAllFibresBeTrusted = false;
    middle_fibre = all_fibres_.begin();
    allFibreSize = 0;

    all_xpoints_.clear();
    canAllXpointsBeTrusted = false;
    middle_xpoint = all_xpoints_.begin();
    allXpointSize = 0;

    all_vertices_.clear();
}

void Datastructures::DFS_visit(XpointPtr &xpoint, bool &found,
                               XpointPtr &destination, Coord& toxpoint)
{
    // Visit xpoints in a depth-first fashion, assigning cost, predecessor and color
    // on the go

    xpoint->color = 0;
    for (auto fibre : xpoint->fibres_){
        auto fibre_ptr = fibre.second;
        Cost current_cost = fibre_ptr->cost_;
        std::shared_ptr next_xpoint = fibre_ptr->x1;
        if (fibre_ptr->x1->coords_ == xpoint->coords_) {
            next_xpoint = fibre_ptr->x2;
        }
        if (next_xpoint->color == -1){
            next_xpoint->predecessor = xpoint;
            next_xpoint->cost_so_far = next_xpoint->predecessor->cost_so_far + current_cost;
            // If reached the destination xpoint, break the loop
            if (next_xpoint->coords_ == toxpoint){
                found = true;
                destination = next_xpoint;
                break;
            }
            DFS_visit(next_xpoint, found, destination, toxpoint);
        }
    }
    xpoint->color = 1;

    if (found){
        return;
    }
    return;
}

std::vector<std::pair<Coord, Cost> > Datastructures::route_any(Coord fromxpoint, Coord toxpoint)
{
    // DFS

    if (all_vertices_.find(fromxpoint) != all_vertices_.end() &&
            all_vertices_.find(toxpoint) != all_vertices_.end()) {
        bool found = false;
        // Initialize all xpoints
        for (auto vertex : all_vertices_){
            auto vertex_ptr = vertex.second;
            vertex_ptr->color = -1;
            vertex_ptr->predecessor = nullptr;
            vertex_ptr->cost_so_far = 0;
        }

        auto from_xpoint = all_vertices_.at(fromxpoint);
        from_xpoint->color = 0;

        XpointPtr destination;
        DFS_visit(from_xpoint, found, destination, toxpoint);

        if (found){
            std::vector<std::pair<Coord, Cost>> any_route;
            while (destination != nullptr){
                auto new_pair = std::make_pair(destination->coords_, destination->cost_so_far);
                any_route.push_back(new_pair);
                destination = destination->predecessor;
            }
            std::reverse(any_route.begin(), any_route.end());
            return any_route;
        } else {
            return {};
        }
    } else {
        return {};
    }
}

std::vector<std::pair<Coord, Cost>> Datastructures::route_least_xpoints(Coord fromxpoint, Coord toxpoint)
{
    // BFS
    if (all_vertices_.find(fromxpoint) != all_vertices_.end() &&
            all_vertices_.find(toxpoint) != all_vertices_.end()) {
        bool found = false;

        for (auto vertex : all_vertices_){
            auto vertex_ptr = vertex.second;
            vertex_ptr->color = -1;
            vertex_ptr->predecessor = nullptr;
            vertex_ptr->cost_so_far = 0;
        }

        auto from_xpoint = all_vertices_.at(fromxpoint);
        from_xpoint->color = 0;

        std::queue<XpointPtr> xpoint_queue = {};
        xpoint_queue.push(from_xpoint);

        XpointPtr destination;

        while (!xpoint_queue.empty()) {
            XpointPtr visiting_xpoint = xpoint_queue.front();
            xpoint_queue.pop();
            for (auto fibre : visiting_xpoint->fibres_){
                auto fibre_ptr = fibre.second;
                Cost current_cost = fibre_ptr->cost_;
                std::shared_ptr next_xpoint = fibre_ptr->x1;
                if (fibre_ptr->x1->coords_ == visiting_xpoint->coords_) {
                    next_xpoint = fibre_ptr->x2;
                }
                if (next_xpoint->color == -1){
                    next_xpoint->color = 0;
                    next_xpoint->predecessor = visiting_xpoint;
                    next_xpoint->cost_so_far = next_xpoint->predecessor->cost_so_far + current_cost;
                    xpoint_queue.push(next_xpoint);
                    if (next_xpoint->coords_ == toxpoint){
                        destination = next_xpoint;
                        found = true;
                        break;
                    }
                }
            }
            visiting_xpoint->color = 1;
            if (found) break;
        }
        if (found){
            std::vector<std::pair<Coord, Cost>> least_route;
            while (destination != nullptr){
                auto new_pair = std::make_pair(destination->coords_, destination->cost_so_far);
                least_route.push_back(new_pair);
                destination = destination->predecessor;
            }
            std::reverse(least_route.begin(), least_route.end());
            return least_route;
        } else {
            return {};
        }
    } else {
        return {};
    }
}

void Datastructures::relax(std::shared_ptr<Datastructures::Xpoint> &prev,
                           std::shared_ptr<Datastructures::Xpoint> &next, Cost &cost_between)
{
    //qDebug() << "Relaxing " << next->coords_.x << "-" << next->coords_.y;
    if (next->cost_so_far > prev->cost_so_far + cost_between){
        //qDebug() << "Cheaper found";
        next->cost_so_far = prev->cost_so_far + cost_between;
        next->predecessor = prev;
    } else if (next->cost_so_far == 0 && next->color != 1){
        //qDebug() << "(likely) Unvisited before";
        next->cost_so_far = prev->cost_so_far + cost_between;
        next->predecessor = prev;
    }
}

std::vector<std::pair<Coord, Cost>> Datastructures::route_fastest(Coord fromxpoint, Coord toxpoint)
{
    if (all_vertices_.find(fromxpoint) != all_vertices_.end() &&
            all_vertices_.find(toxpoint) != all_vertices_.end()) {
        // Dijkstra
        bool found = false;

        for (auto vertex : all_vertices_){
            auto vertex_ptr = vertex.second;
            vertex_ptr->color = -1;
            vertex_ptr->predecessor = nullptr;
            vertex_ptr->cost_so_far = 0;
        }

        std::priority_queue<XpointCostPair, std::vector<XpointCostPair>, std::greater<XpointCostPair>> cost_queue;
        XpointPtr destination;

        auto from_xpoint = all_vertices_.at(fromxpoint);
        from_xpoint->color = 0;
        cost_queue.push(std::make_pair(0, from_xpoint));
        //qDebug() << "Start from: " << from_xpoint->coords_.x << "-" << from_xpoint->coords_.y;
        while (!cost_queue.empty()){
            XpointPtr visiting_xpoint = cost_queue.top().second;
            cost_queue.pop();
            //qDebug() << "Took from queue: " << visiting_xpoint->coords_.x << "-" << visiting_xpoint->coords_.y;
            for (auto fibre : visiting_xpoint->fibres_){
                auto fibre_ptr = fibre.second;
                Cost current_cost = fibre_ptr->cost_;
                std::shared_ptr next_xpoint= fibre_ptr->x1;
                if (fibre_ptr->x1->coords_ == visiting_xpoint->coords_) {
                    next_xpoint = fibre_ptr->x2;
                }
                //qDebug() << "Reached " << next_xpoint->coords_.x << "-" << next_xpoint->coords_.y;
                if (next_xpoint->color == -1){
                    next_xpoint->color = 0;
                    cost_queue.push(std::make_pair(current_cost, next_xpoint));
                    //qDebug() << "Pushed to queue: " << next_xpoint->coords_.x << "-" << next_xpoint->coords_.y;
                    if (next_xpoint->coords_ == toxpoint){
                        destination = next_xpoint;
                        found = true;
                        //qDebug() << "Found it!";
                    }
                }
                relax(visiting_xpoint, next_xpoint, current_cost);
            }
            visiting_xpoint->color = 1;
            //qDebug() << "Marked black: " << visiting_xpoint->coords_.x << "-" << visiting_xpoint->coords_.y;
        }
        if (found){
            std::vector<std::pair<Coord, Cost>> fastest_route;
            while (destination != nullptr){
                auto new_pair = std::make_pair(destination->coords_, destination->cost_so_far);
                fastest_route.push_back(new_pair);
                destination = destination->predecessor;
            }
            std::reverse(fastest_route.begin(), fastest_route.end());
            return fastest_route;
        } else {
            return {};
        }
    } else {
        return {};
    }
}

bool Datastructures::find_cycle(XpointPtr next,
                                XpointPtr prev,
                                XpointPtr &cycle_begin)
{
    //qDebug() << "Checking " << next->coords_.x << "-" << next->coords_.y;
    next->color = 0;

    for (auto fibre : next->fibres_){
        auto fibre_ptr = fibre.second;
        std::shared_ptr next_xpoint = fibre_ptr->x1;
        if (fibre_ptr->x1->coords_ == next->coords_) {
            next_xpoint = fibre_ptr->x2;
        }
        if (next_xpoint->color == -1){
            //qDebug() << "Visiting " << next_xpoint->coords_.x << "-" << next_xpoint->coords_.y;
            next_xpoint->predecessor = next;
            if(find_cycle(next_xpoint, next, cycle_begin)){
                return true;
            }
        } else if (next_xpoint != prev){
            //qDebug() << "Found it";
            next_xpoint->predecessor = next;
            cycle_begin = next_xpoint;
            return true;
        }
    }
    return false;
}

std::vector<Coord> Datastructures::route_fibre_cycle(Coord startxpoint)
{
    if (all_vertices_.find(startxpoint) != all_vertices_.end()) {
        // DFS
        bool found = false;
        XpointPtr cycle_begin;
        Coord begin_coord;

        for (auto vertex : all_vertices_){
            auto vertex_ptr = vertex.second;
            vertex_ptr->color = -1;
            vertex_ptr->predecessor = nullptr;
        }

        auto start_xpoint = all_vertices_.at(startxpoint);
        start_xpoint->color = 0;

        for (auto fibre : start_xpoint->fibres_){
            auto fibre_ptr = fibre.second;
            std::shared_ptr next_xpoint = fibre_ptr->x1;
            if (fibre_ptr->x1->coords_ == start_xpoint->coords_) {
                next_xpoint = fibre_ptr->x2;
            }
            if (next_xpoint->color == -1){
                next_xpoint->predecessor = start_xpoint;
                found = find_cycle(next_xpoint, start_xpoint, cycle_begin);
                if (found){
                    break;
                }
            }
        }

        if (found) {
            begin_coord = cycle_begin->coords_;
            //qDebug() << "Found cycle begins at " << begin_coord.x << "-" << begin_coord.y;
            std::vector<Coord> cycle_route;
            while (cycle_begin != nullptr){
                //qDebug() << "Printing " << cycle_begin->coords_.x << "-" << cycle_begin->coords_.y;
                cycle_route.push_back(cycle_begin->coords_);
                if (cycle_begin->color == 1){
                    break;
                } else {
                    cycle_begin->color = 1;
                    cycle_begin = cycle_begin->predecessor;
                    //qDebug() << "Next gonna be: " << cycle_begin->coords_.x << "-" << cycle_begin->coords_.y;
                }
            }
            std::reverse(cycle_route.begin(), cycle_route.end());
            return cycle_route;
        } else {
            return {};
        }
    } else {
        return {};
    }
}

void Datastructures::link_roots(Datastructures::XpointPtr first, Datastructures::XpointPtr second)
{
    //qDebug() << "Linking " << first->coords_.x << "-" << first->coords_.y << "and" << second->coords_.x << "-" << second->coords_.y;
    if (first->rank > second->rank){
        //qDebug() << "First rank is bigger than second";
        second->predecessor = first;
    } else {
        //qDebug() << "Second rank is bigger than first";
        first->predecessor = second;
        if (first->rank == second->rank){
            //qDebug() << "Equal, increment rank";
            second->rank += 1;
        }
    }
}

Datastructures::XpointPtr Datastructures::find_root(Datastructures::XpointPtr xpoint)
{
    //qDebug() << "Find roots of " << xpoint->coords_.x << "-" << xpoint->coords_.y;
    if (xpoint->predecessor != xpoint){
        xpoint->predecessor = find_root(xpoint->predecessor);
    }
    //qDebug() << "Done finding roots of " << xpoint->coords_.x << "-" << xpoint->coords_.y;
    //qDebug() << "Roots of " << xpoint->coords_.x << "-" << xpoint->coords_.y << "is" << xpoint->predecessor->coords_.x << "-" << xpoint->predecessor->coords_.y;
    return xpoint->predecessor;
}

Cost Datastructures::trim_fibre_network()
{
    // deal with all_fibres (removals)
    Cost total_cost = 0;

    for (auto vertex : all_vertices_){
        auto vertex_ptr = vertex.second;
        vertex_ptr->rank = 0;
        vertex_ptr->predecessor = vertex_ptr;
        //qDebug() << "Parent of " << vertex_ptr->coords_.x << "-" << vertex_ptr->coords_.y << "is" << vertex_ptr->predecessor->coords_.x << "-" << vertex_ptr->predecessor->coords_.y;
    }

    // Add fibres infos to priority queue
    std::priority_queue<FibreCostPair, std::vector<FibreCostPair>, std::greater<FibreCostPair>> costly_fibres_queue;
    for (auto fibre_pair : all_fibres_){
        Coord xpoint1 = fibre_pair.first;
        Coord xpoint2 = fibre_pair.second;
        XpointPtr x1 = all_vertices_.at(xpoint1);
        FibrePtr fibre_ptr = x1->fibres_.at(xpoint2);
        Cost new_cost = fibre_ptr->cost_;
        costly_fibres_queue.push(std::make_pair(new_cost, fibre_ptr));
    }

    all_fibres_.clear();
    canAllFibresBeTrusted = false;
    allFibreSize = 0;

    while (!costly_fibres_queue.empty()){
        FibreCostPair fibre_info_pair = costly_fibres_queue.top();
        costly_fibres_queue.pop();

        Cost current_cost = fibre_info_pair.first;

        XpointPtr first = fibre_info_pair.second->x1;
        XpointPtr second = fibre_info_pair.second->x2;

        //qDebug() << "Fibre between " << first->coords_.x << "-" << first->coords_.y << "and" << second->coords_.x << "-" << second->coords_.y;
        //qDebug() << "Next current cost is" << current_cost;

        XpointPtr first_root = find_root(first);
        XpointPtr second_root = find_root(second);

        Coord x1_coord = first->coords_;
        Coord x2_coord = second->coords_;

        if (first_root != second_root){
            //qDebug() << "Diffrent set";
            all_fibres_.push_back(std::make_pair(x1_coord, x2_coord));
            //qDebug() << "9";
            total_cost += current_cost;
            link_roots(first_root, second_root);
            //qDebug() << "14";
        } else {
            remove_fibre(x1_coord, x2_coord);
            //qDebug() << "Same set";
        }
    }
    //qDebug() << "15";
    return total_cost;
}
