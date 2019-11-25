// Datastructures.hh

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <utility>
#include <limits>

//Added by students
#include <unordered_map>
#include <memory>

// Type for beacon IDs
using BeaconID = std::string;

// Return value for cases where required beacon was not found
BeaconID const NO_ID = "----------";

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Return value for cases where name values were not found
std::string const NO_NAME = "-- unknown --";

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};

// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

struct CoordHash
{
    std::size_t operator()(Coord xy) const
    {
        auto hasher = std::hash<int>();
        auto xhash = hasher(xy.x);
        auto yhash = hasher(xy.y);
        // Combine hash values (magic!)
        return xhash ^ (yhash + 0x9e3779b9 + (xhash << 6) + (xhash >> 2));
    }
};

// Example: Defining < for Coord so that it can be used
// as key for std::map/set
inline bool operator<(Coord c1, Coord c2)
{
    if (c1.y < c2.y) { return true; }
    else if (c2.y < c1.y) { return false; }
    else { return c1.x < c2.x; }
}

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

// Type for color (RGB)
struct Color
{
    int r = NO_VALUE;
    int g = NO_VALUE;
    int b = NO_VALUE;
};

// Equality and non-equality comparisons for Colors
inline bool operator==(Color c1, Color c2) { return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b; }
inline bool operator!=(Color c1, Color c2) { return !(c1==c2); }

// Return value for cases where color was not found
Color const NO_COLOR = {NO_VALUE, NO_VALUE, NO_VALUE};

// Type for light transmission cost (used only in the second assignment)
using Cost = int;

// Return value for cases where cost is unknown
Cost const NO_COST = NO_VALUE;

// This is the class you are supposed to implement

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: O(1)
    // Short rationale for estimate: the function .size() only takes constant time for unordered_map
    int beacon_count();

    // Estimate of performance: O(1)
    // Short rationale for estimate: function clear() takes constant time,
    // set bool to false also constant time
    void clear_beacons();

    // Estimate of performance: Theta(n), O(n^2) - worst case rarely happens if hash function is good
    // Short rationale for estimate: a for loop obtaining element from unordered_map
    // takes linear time on average because accessing takes constant time.
    std::vector<BeaconID> all_beacons();

    // Estimate of performance: Theta(1), O(n) - worst case rarely happens if hash function is good
    // Short rationale for estimate: on average, function find() and insert()
    // on unordered_map takes constant time, but might be linear in worst case
    bool add_beacon(BeaconID id, const std::string& name, Coord xy, Color color);

    // Estimate of performance: Theta(1), O(n) - worst case rarely happens if hash function is good
    // Short rationale for estimate: on average, function find() and at()
    // on unordered_map takes constant time, but might be linear in worst case
    std::string get_name(BeaconID id);

    // Estimate of performance: Theta(1), O(n) - worst case rarely happens if hash function is good
    // Short rationale for estimate: on average, function find() and at()
    // on unordered_map takes constant time, but might be linear in worst case
    Coord get_coordinates(BeaconID id);

    // Estimate of performance: Theta(1), O(n) - worst case rarely happens if hash function is good
    // Short rationale for estimate: on average, function find() and at()
    // on unordered_map takes constant time, but might be linear in worst case
    Color get_color(BeaconID id);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: O(nlogn), Omega(1)
    // Short rationale for estimate: std::sort takes O(nlogn). However if we ran this function before
    // and nothing changes, we can just return it
    std::vector<BeaconID> beacons_alphabetically();

    // Estimate of performance: O(nlogn), Omega(1)
    // Short rationale for estimate: std::sort takes O(nlogn). However if we ran this function before
    // and nothing changes, we can just return it
    std::vector<BeaconID> beacons_brightness_increasing();

    // Estimate of performance: Omega(1), worst case O(nlogn)
    // Short rationale for estimate: Best case: we obtained dimmest beacon if we sorted beacons
    // according to brightness and that beacon is still the dimmest. Otherwise we have to sort again
    BeaconID min_brightness();

    // Estimate of performance: Omega(1), O(nlogn)
    // Short rationale for estimate:Best case: we obtained brightest beacon if we sorted beacons
    // according to brightness and that beacon is still the brightest. Otherwise we have to sort again
    BeaconID max_brightness();

    // Estimate of performance: O(nlogn)
    // Short rationale for estimate: We have to tranverse through all elements which takes linear time
    // Sorting the beacons according to ID takes nlogn
    std::vector<BeaconID> find_beacons(std::string const& name);

    // Estimate of performance: Theta(1), O(n) - worst case rarely happens if hash function is good
    // Short rationale for estimate: find() and at() for unordered_map takes constant time on average
    // linear in the worst case
    bool change_beacon_name(BeaconID id, std::string const& newname);

    // Estimate of performance: Theta(1), O(n) - worst case rarely happens if hash function is good
    // Short rationale for estimate: find() and at() for unordered_map takes constant time on average
    // linear in the worst case
    bool change_beacon_color(BeaconID id, Color newcolor);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: Theta(1), O(n) - worst case rarely happens if hash function is good
    // Short rationale for estimate: We insert the source beacons pointers to the vector contains
    // all sources while keeping the order. This requires finding the place for the new element
    // using std::upper_bound(), which takes O(n).
    bool add_lightbeam(BeaconID sourceid, BeaconID targetid);

    // Estimate of performance: Omega(1), O(nlogn)
    // Short rationale for estimate: If the lightsources of that beacon is was not modified since we
    // last sorted it by calling this function, then it can be returned directly. Otherwise sorting
    // takes nlogn
    std::vector<BeaconID> get_lightsources(BeaconID id);

    // Estimate of performance: Theta(n), O(n^2) - worst case rarely happens if hash function is good
    // Short rationale for estimate: Accessing the 'target' element takes constant takes constant time
    // on average or linear in worst case, but we have to traverse through all the 'targets', which
    // takes linear or n^2, respectively
    std::vector<BeaconID> path_outbeam(BeaconID id);

    // Non-compulsory operations

    // Estimate of performance: Theta(n), O(n^2) - worst case rarely happens if hash function is good
    // Short rationale for estimate: We also have to erase the beacon as source of another beacon and
    // target of other beacons. Erase-remove idiom takes linear time on average but might take n^2 in
    // worst case (std::remove takes linear, erase() on unordered_map takes Theta(1), O(n)).
    bool remove_beacon(BeaconID id);

    // Estimate of performance: O(n), but perftests show that O(logn) might be a better fit (recursive func)
    // Short rationale for estimate: We have to call private-defined functions 'height' and 'find_longest'
    // to find the tree's height and longest inbeam, respectively, which both take O(n)
    std::vector<BeaconID> path_inbeam_longest(BeaconID id);

    // Estimate of performance: O(n), but perftests show that O(logn) might be a better fit (recursive func)
    // Short rationale for estimate: We have to call private-defined function 'average' to obtain the
    // final color, which takes linear time.
    Color total_color(BeaconID id);

private:
    // Data structures and variables

    // struct Beacon holds information about the beacon's info and also acts
    // as a node in a tree, needed to calculate lightbeams' colors.
    struct Beacon {
        BeaconID id; // seems unnecessary but will be needed
        std::string name;
        Color color;
        Coord coord;
        std::vector<BeaconID> sources_;
        BeaconID target_;
        std::vector<std::shared_ptr<Beacon>> sources_nodes_;
        bool isSourcesSorted;
    };

    // Unorderd_map contains beacon IDs as keys and their pointers as values
    std::unordered_map<BeaconID, std::shared_ptr<Beacon>> id_data;
    bool isBrightnessSorted;
    bool isNameSorted;
    bool isBrightestValid;
    bool isDimmestValid;
    std::pair <BeaconID, int> min_bright;
    std::pair <BeaconID, int> max_bright;

    // Vector sorted by name that will be returned
    std::vector<BeaconID> beacon_alpha;
    // Vector sorted by brightness that will be returned
    std::vector<BeaconID> beacon_brightness;

    // Functions

    // Estimate of performance: O(1)
    // Short rationale for estimate: sheer calculations, no loop
    int get_brightness(Color color);

    // Estimate of performance: O(n)
    // Short rationale for estimate: We have to traverse through all nodes to count the height in worst case
    unsigned long height (std::shared_ptr<Beacon> beacon);

    // Estimate of performance: O(n)
    // Short rationale for estimate: We have to traverse through all nodes in worst case
    void find_longest (std::shared_ptr<Beacon> beacon, const unsigned long h,
                       std::vector<BeaconID> &inbeam, BeaconID id, unsigned long index);

    // Estimate of performance: O(n)
    // Short rationale for estimate: We have to traverse through the nodes to get the color and calculate
    // the average color for the lightbeam
    Color average (std::shared_ptr<Beacon> beacon);

};

#endif // DATASTRUCTURES_HH
