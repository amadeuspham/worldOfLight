// Datastructures.cc

#include "datastructures.hh"

#include <random>
#include <cmath>

// Added by students
#include <memory>
#include <unordered_map>
#include <algorithm>

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
}

Datastructures::~Datastructures()
{
    clear_beacons();
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

bool Datastructures::remove_beacon(BeaconID id) // error when removing a target only beacon
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
