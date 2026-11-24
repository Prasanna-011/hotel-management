
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <iomanip>
using namespace std;

// ================= ROOM CLASS =================
class Room {
private:
    int roomNumber;
    string roomType;
    double price;

public:
    Room(int number = 0, string type = "", double cost = 0) {
        roomNumber = number;
        roomType = type;
        price = cost;
    }

    int getRoomNumber() const {
        return roomNumber;
    }

    string getRoomType() const {
        return roomType;
    }

    double getPrice() const {
        return price;
    }

    void display() const {
        cout << left << setw(12) << roomNumber
             << setw(15) << roomType
             << "Rs. " << price << endl;
    }
};

// ================= CUSTOMER CLASS =================
class Customer {
private:
    string name;
    string phone;

public:
    Customer(string n = "", string p = "") {
        name = n;
        phone = p;
    }

    string getName() const {
        return name;
    }

    string getPhone() const {
        return phone;
    }
};

// ================= BOOKING CLASS =================
class Booking {
private:
    int bookingID;
    int roomNumber;
    Customer customer;
    string checkIn;
    string checkOut;
    bool active;

public:
    Booking(int id, int room, Customer c,
            string in, string out, bool status = true) {
        bookingID = id;
        roomNumber = room;
        customer = c;
        checkIn = in;
        checkOut = out;
        active = status;
    }

    int getBookingID() const {
        return bookingID;
    }

    int getRoomNumber() const {
        return roomNumber;
    }

    string getCustomerName() const {
        return customer.getName();
    }

    string getPhone() const {
        return customer.getPhone();
    }

    string getCheckIn() const {
        return checkIn;
    }

    string getCheckOut() const {
        return checkOut;
    }

    bool isActive() const {
        return active;
    }

    void checkout() {
        active = false;
    }

    void display() const {
        cout << "\nBooking ID : " << bookingID;
        cout << "\nRoom Number: " << roomNumber;
        cout << "\nCustomer   : " << customer.getName();
        cout << "\nPhone      : " << customer.getPhone();
        cout << "\nCheck-in   : " << checkIn;
        cout << "\nCheck-out  : " << checkOut;
        cout << "\nStatus     : "
             << (active ? "Booked" : "Checked Out");
        cout << "\n-----------------------------\n";
    }
};

// ================= HOTEL CLASS =================
class Hotel {
private:
    vector<Room> rooms;
    vector<Booking> bookings;
    int nextBookingID;

    // Save room records
    void saveRooms() {
        ofstream file("rooms.txt");

        if (!file) {
            cout << "Error saving room records!\n";
            return;
        }

        for (const Room& r : rooms) {
            file << r.getRoomNumber() << "|"
                 << r.getRoomType() << "|"
                 << r.getPrice() << endl;
        }

        file.close();
    }

    // Save booking and customer records
    void saveBookings() {
        ofstream file("bookings.txt");

        if (!file) {
            cout << "Error saving booking records!\n";
            return;
        }

        for (const Booking& b : bookings) {
            file << b.getBookingID() << "|"
                 << b.getRoomNumber() << "|"
                 << b.getCustomerName() << "|"
                 << b.getPhone() << "|"
                 << b.getCheckIn() << "|"
                 << b.getCheckOut() << "|"
                 << b.isActive() << endl;
        }

        file.close();
    }

    // Check whether room exists
    bool roomExists(int number) const {
        for (const Room& r : rooms) {
            if (r.getRoomNumber() == number) {
                return true;
            }
        }
        return false;
    }

    // Find room details
    const Room* findRoom(int number) const {
        for (const Room& r : rooms) {
            if (r.getRoomNumber() == number) {
                return &r;
            }
        }
        return nullptr;
    }

    // Check date overlap (YYYY-MM-DD format)
    bool isRoomAvailable(int room, string in, string out) const {
        for (const Booking& b : bookings) {
            if (b.getRoomNumber() == room && b.isActive()) {

                bool overlap =
                    (in < b.getCheckOut() &&
                     out > b.getCheckIn());

                if (overlap) {
                    return false;
                }
            }
        }

        return true;
    }

    // Validate date format
    bool validDate(string date) const {
        if (date.length() != 10 ||
            date[4] != '-' || date[7] != '-') {
            return false;
        }

        for (int i = 0; i < 10; i++) {
            if (i == 4 || i == 7)
                continue;

            if (date[i] < '0' || date[i] > '9')
                return false;
        }

        int month = stoi(date.substr(5, 2));
        int day = stoi(date.substr(8, 2));

        return month >= 1 && month <= 12 &&
               day >= 1 && day <= 31;
    }

public:
    Hotel() {
        nextBookingID = 1;
        loadData();
    }

    // Load room and booking data
    void loadData() {
        rooms.clear();
        bookings.clear();

        ifstream roomFile("rooms.txt");
        string line;

        while (getline(roomFile, line)) {
            stringstream ss(line);
            string number, type, price;

            if (getline(ss, number, '|') &&
                getline(ss, type, '|') &&
                getline(ss, price)) {
                try {
                    rooms.emplace_back(
                        stoi(number), type, stod(price)
                    );
                }
                catch (...) {
                    continue;
                }
            }
        }

        roomFile.close();

        ifstream bookingFile("bookings.txt");

        while (getline(bookingFile, line)) {
            stringstream ss(line);

            string id, room, name, phone;
            string in, out, status;

            if (getline(ss, id, '|') &&
                getline(ss, room, '|') &&
                getline(ss, name, '|') &&
                getline(ss, phone, '|') &&
                getline(ss, in, '|') &&
                getline(ss, out, '|') &&
                getline(ss, status)) {

                try {
                    int bookingId = stoi(id);

                    bookings.emplace_back(
                        bookingId,
                        stoi(room),
                        Customer(name, phone),
                        in,
                        out,
                        status == "1"
                    );

                    if (bookingId >= nextBookingID) {
                        nextBookingID = bookingId + 1;
                    }
                }
                catch (...) {
                    continue;
                }
            }
        }

        bookingFile.close();
    }

    // Add default rooms if no room data exists
    void initializeRooms() {
        if (!rooms.empty()) {
            cout << "\nRoom records already exist.\n";
            return;
        }

        rooms.emplace_back(101, "Single", 1500);
        rooms.emplace_back(102, "Double", 2500);
        rooms.emplace_back(103, "Deluxe", 3500);
        rooms.emplace_back(104, "Suite", 5000);
        rooms.emplace_back(105, "Single", 1500);

        saveRooms();

        cout << "\nDefault rooms added successfully!\n";
    }

    // Display all rooms
    void displayRooms() const {
        cout << "\n========== ROOM DETAILS ==========\n";

        cout << left << setw(12) << "Room No."
             << setw(15) << "Room Type"
             << "Price Per Night\n";

        cout << "----------------------------------\n";

        for (const Room& r : rooms) {
            r.display();
        }
    }

    // Display currently available rooms for dates
    void availableRooms() {
        string in, out;

        cout << "\nEnter check-in date (YYYY-MM-DD): ";
        cin >> in;

        cout << "Enter check-out date (YYYY-MM-DD): ";
        cin >> out;

        if (!validDate(in) || !validDate(out) || in >= out) {
            cout << "\nInvalid dates! Please check the date format and range.\n";
            return;
        }

        cout << "\nAvailable Rooms:\n";

        bool found = false;

        for (const Room& r : rooms) {
            if (isRoomAvailable(r.getRoomNumber(), in, out)) {
                r.display();
                found = true;
            }
        }

        if (!found) {
            cout << "No rooms available for these dates.\n";
        }
    }

    // Book a room
    void bookRoom() {
        int room;
        string name, phone, in, out;

        cout << "\nEnter Room Number: ";
        cin >> room;

        if (!roomExists(room)) {
            cout << "Room does not exist!\n";
            return;
        }

        cout << "Enter Customer Name: ";
        cin >> ws;
        getline(cin, name);

        cout << "Enter Phone Number: ";
        getline(cin, phone);

        cout << "Enter Check-in Date (YYYY-MM-DD): ";
        cin >> in;

        cout << "Enter Check-out Date (YYYY-MM-DD): ";
        cin >> out;

        if (name.empty() || phone.empty()) {
            cout << "Customer details cannot be empty!\n";
            return;
        }

        if (!validDate(in) || !validDate(out) || in >= out) {
            cout << "Invalid dates!\n";
            return;
        }

        if (!isRoomAvailable(room, in, out)) {
            cout << "\nSorry! This room is already booked for those dates.\n";
            return;
        }

        Booking newBooking(
            nextBookingID++,
            room,
            Customer(name, phone),
            in,
            out
        );

        bookings.push_back(newBooking);

        saveBookings();

        cout << "\nRoom booked successfully!\n";
        cout << "Booking ID: "
             << newBooking.getBookingID() << endl;

        newBooking.display();
    }

    // Checkout customer
    void checkoutCustomer() {
        int id;

        cout << "\nEnter Booking ID for checkout: ";
        cin >> id;

        for (Booking& b : bookings) {
            if (b.getBookingID() == id && b.isActive()) {

                b.checkout();

                saveBookings();

                cout << "\nCheckout completed successfully!\n";
                return;
            }
        }

        cout << "\nActive booking not found!\n";
    }

    // Search customer
    void searchCustomer() {
        string search;

        cout << "\nEnter customer name or phone number: ";
        cin >> ws;
        getline(cin, search);

        bool found = false;

        for (const Booking& b : bookings) {
            if (b.getCustomerName().find(search) != string::npos ||
                b.getPhone().find(search) != string::npos) {

                b.display();
                found = true;
            }
        }

        if (!found) {
            cout << "\nCustomer record not found!\n";
        }
    }

    // Display all bookings
    void displayBookings() const {
        cout << "\n========== ALL BOOKINGS ==========\n";

        if (bookings.empty()) {
            cout << "No bookings available.\n";
            return;
        }

        for (const Booking& b : bookings) {
            b.display();
        }
    }
};

// ================= MAIN FUNCTION =================
int main() {
    Hotel hotel;
    int choice;

    cout << "====================================\n";
    cout << "       HOTEL MANAGEMENT SYSTEM\n";
    cout << "====================================\n";

    hotel.initializeRooms();

    do {
        cout << "\n========== MAIN MENU ==========\n";
        cout << "1. Display All Rooms\n";
        cout << "2. Check Available Rooms\n";
        cout << "3. Book a Room\n";
        cout << "4. Customer Checkout\n";
        cout << "5. Search Customer\n";
        cout << "6. Display All Bookings\n";
        cout << "7. Exit\n";
        cout << "===============================\n";

        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input! Enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1:
                hotel.displayRooms();
                break;

            case 2:
                hotel.availableRooms();
                break;

            case 3:
                hotel.bookRoom();
                break;

            case 4:
                hotel.checkoutCustomer();
                break;

            case 5:
                hotel.searchCustomer();
                break;

            case 6:
                hotel.displayBookings();
                break;

            case 7:
                cout << "\nThank you for using Hotel Management System!\n";
                break;

            default:
                cout << "\nInvalid choice! Please select 1-7.\n";
        }

    } while (choice != 7);

    return 0;
}
