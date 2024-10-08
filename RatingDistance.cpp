#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <chrono>

struct Rating
{
    int movieID;
    double rating;
};

void loadUserRatings(const std::string &filename, int userID1, int userID2, std::map<int, double> &userRating1, std::map<int, double> &userRating2)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string userID_str, movieID_str, rating_str, timestamp;

        std::getline(ss, userID_str, ':');
        ss.ignore(1); // Ignore the extra ':'
        std::getline(ss, movieID_str, ':');
        ss.ignore(1); // Ignore the extra ':'
        std::getline(ss, rating_str, ':');
        ss.ignore(1); // Ignore the extra ':'
        std::getline(ss, timestamp);

        int user = std::stoi(userID_str);
        int movieID = std::stoi(movieID_str);
        double rating = std::stod(rating_str);

        if (user == userID1)
        {
            userRating1[movieID] = rating;
        }
        if (user == userID2)
        {
            userRating2[movieID] = rating;
        }
    }
}

double calculateRatingDistance(const std::map<int, double> &user1Ratings, const std::map<int, double> &user2Ratings)
{
    double totalDistance = 0;
    int commonMovies = 0;

    for (std::map<int, double>::const_iterator it = user1Ratings.begin(); it != user1Ratings.end(); ++it)
    {
        int movieID = it->first;
        double rating1 = it->second;
        // add distance only if both users have seen this movie
        if (user2Ratings.find(movieID) != user2Ratings.end())
        {
            double rating2 = user2Ratings.at(movieID);
            std::cout << "movieid: " << movieID << ", rating1:" << rating1 << ", rating2:" << rating2 << std::endl;
            totalDistance += std::abs(rating1 - rating2);
            commonMovies++;
        }
    }

    // no common movies
    if (commonMovies == 0)
    {
        return -1.0;
    }

    return totalDistance / commonMovies;
}

// ./RatingDistance 13 33680
// ./RatingDistance_2 453 10425
int main(int argc, char *argv[])
{
    // Start time point
    auto start = std::chrono::high_resolution_clock::now();

    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " [UserID1] [UserID2]" << std::endl;
        return 1;
    }

    int userID1 = std::stoi(argv[1]);
    int userID2 = std::stoi(argv[2]);

    std::string ratingsFile = "ratings.dat";

    // Load the ratings for both users
    std::map<int, double> user1Ratings, user2Ratings;
    loadUserRatings(ratingsFile, userID1, userID2, user1Ratings, user2Ratings);

    // Compute the rating distance
    double distance = calculateRatingDistance(user1Ratings, user2Ratings);

    // Output the result
    if (distance < 0)
    {
        std::cout << "The two users have no common movies." << std::endl;
    }
    else
    {
        std::cout << "The RatingDistance is " << distance << std::endl;
    }

    // End time point
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    std::chrono::duration<double> elapsed = end - start;

    // Output the time taken
    std::cout << "Time taken by main() function: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}