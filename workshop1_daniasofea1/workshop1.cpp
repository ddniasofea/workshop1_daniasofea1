#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

using namespace std;

// ================= BILL CALCULATION =================
double calculateBill(int age, int medicineQty, int mcDays) {
    double consultation = 50;
    double medicine = medicineQty * 20;
    double injection = 30;
    double mcCharge = mcDays * 5;

    double total = consultation + medicine + injection + mcCharge;

    // 20% discount for senior citizens
    if (age >= 60) {
        total -= total * 0.20;
    }

    return total;
}

// ================= MAIN PROGRAM =================
int main() {

    try {
        sql::mysql::MySQL_Driver* driver;
        sql::Connection* con;

        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "root", "");
        con->setSchema("clinic_db");

        int role;

        while (true) {
            cout << "\n===== LOGIN MENU =====\n";
            cout << "1. Admin\n";
            cout << "2. User\n";
            cout << "3. Exit\n";
            cout << "Choose: ";
            cin >> role;

            // ================= ADMIN =================
            if (role == 1) {

                string password;
                cout << "Enter Admin Password: ";
                cin >> password;

                if (password != "admin123") {
                    cout << "Wrong password!\n";
                    continue;
                }

                int choice;
                while (true) {
                    cout << "\n===== ADMIN MENU =====\n";
                    cout << "1. Add Patient\n";
                    cout << "2. Delete Patient\n";
                    cout << "3. Edit Patient\n";
                    cout << "4. Search Patient\n";
                    cout << "5. Display All\n";
                    cout << "6. Logout\n";
                    cout << "Choose: ";
                    cin >> choice;

                    // ADD
                    if (choice == 1) {
                        int id, age, medicineQty, mcDays;
                        string name, disease;

                        cout << "ID: "; cin >> id;
                        cout << "Name: "; cin >> name;
                        cout << "Age: "; cin >> age;
                        cout << "Disease: "; cin >> disease;
                        cout << "Medicine Quantity: "; cin >> medicineQty;
                        cout << "MC Days: "; cin >> mcDays;

                        double total = calculateBill(age, medicineQty, mcDays);

                        sql::PreparedStatement* pstmt;
                        pstmt = con->prepareStatement(
                            "INSERT INTO patients VALUES (?, ?, ?, ?, ?, ?, ?)"
                        );

                        pstmt->setInt(1, id);
                        pstmt->setString(2, name);
                        pstmt->setInt(3, age);
                        pstmt->setString(4, disease);
                        pstmt->setInt(5, medicineQty);
                        pstmt->setInt(6, mcDays);
                        pstmt->setDouble(7, total);

                        pstmt->execute();
                        delete pstmt;

                        cout << "Patient Added! Total Bill: RM" << total << endl;
                    }

                    // DELETE
                    else if (choice == 2) {
                        int id;
                        cout << "Enter ID to delete: ";
                        cin >> id;

                        sql::PreparedStatement* pstmt;
                        pstmt = con->prepareStatement(
                            "DELETE FROM patients WHERE id=?"
                        );
                        pstmt->setInt(1, id);
                        pstmt->execute();
                        delete pstmt;

                        cout << "Patient Deleted.\n";
                    }

                    // EDIT
                    else if (choice == 3) {
                        int id, medicineQty, mcDays;
                        string disease;

                        cout << "Enter ID to edit: ";
                        cin >> id;

                        sql::PreparedStatement* pstmt;
                        sql::ResultSet* res;

                        pstmt = con->prepareStatement(
                            "SELECT age FROM patients WHERE id=?"
                        );
                        pstmt->setInt(1, id);
                        res = pstmt->executeQuery();

                        if (!res->next()) {
                            cout << "Patient not found.\n";
                            delete res;
                            delete pstmt;
                            continue;
                        }

                        int age = res->getInt("age");
                        delete res;
                        delete pstmt;

                        cout << "New Disease: ";
                        cin >> disease;
                        cout << "New Medicine Qty: ";
                        cin >> medicineQty;
                        cout << "New MC Days: ";
                        cin >> mcDays;

                        double total = calculateBill(age, medicineQty, mcDays);

                        pstmt = con->prepareStatement(
                            "UPDATE patients SET disease=?, medicineQty=?, mcDays=?, totalBill=? WHERE id=?"
                        );

                        pstmt->setString(1, disease);
                        pstmt->setInt(2, medicineQty);
                        pstmt->setInt(3, mcDays);
                        pstmt->setDouble(4, total);
                        pstmt->setInt(5, id);

                        pstmt->execute();
                        delete pstmt;

                        cout << "Updated! New Bill: RM" << total << endl;
                    }

                    // SEARCH
                    else if (choice == 4) {
                        int id;
                        cout << "Enter ID: ";
                        cin >> id;

                        sql::PreparedStatement* pstmt;
                        sql::ResultSet* res;

                        pstmt = con->prepareStatement(
                            "SELECT * FROM patients WHERE id=?"
                        );
                        pstmt->setInt(1, id);
                        res = pstmt->executeQuery();

                        if (res->next()) {
                            cout << "\nID: " << res->getInt("id");
                            cout << "\nName: " << res->getString("name");
                            cout << "\nAge: " << res->getInt("age");
                            cout << "\nDisease: " << res->getString("disease");
                            cout << "\nTotal Bill: RM" << res->getDouble("totalBill") << endl;
                        }
                        else {
                            cout << "Patient not found.\n";
                        }

                        delete res;
                        delete pstmt;
                    }

                    // DISPLAY ALL
                    else if (choice == 5) {
                        sql::Statement* stmt = con->createStatement();
                        sql::ResultSet* res = stmt->executeQuery("SELECT * FROM patients");

                        cout << "\n===== Patient List =====\n";
                        while (res->next()) {
                            cout << res->getInt("id") << " | "
                                << res->getString("name") << " | "
                                << res->getInt("age") << " | "
                                << res->getString("disease") << " | RM"
                                << res->getDouble("totalBill") << endl;
                        }

                        delete res;
                        delete stmt;
                    }

                    else if (choice == 6) {
                        break;
                    }
                }
            }

            // ================= USER =================
            else if (role == 2) {

                int choice;
                while (true) {
                    cout << "\n===== USER MENU =====\n";
                    cout << "1. Search Patient\n";
                    cout << "2. Display All\n";
                    cout << "3. Logout\n";
                    cout << "Choose: ";
                    cin >> choice;

                    if (choice == 1) {
                        int id;
                        cout << "Enter ID: ";
                        cin >> id;

                        sql::PreparedStatement* pstmt;
                        sql::ResultSet* res;

                        pstmt = con->prepareStatement(
                            "SELECT * FROM patients WHERE id=?"
                        );
                        pstmt->setInt(1, id);
                        res = pstmt->executeQuery();

                        if (res->next()) {
                            cout << "\nID: " << res->getInt("id");
                            cout << "\nName: " << res->getString("name");
                            cout << "\nAge: " << res->getInt("age");
                            cout << "\nDisease: " << res->getString("disease");
                            cout << "\nTotal Bill: RM" << res->getDouble("totalBill") << endl;
                        }
                        else {
                            cout << "Patient not found.\n";
                        }

                        delete res;
                        delete pstmt;
                    }

                    else if (choice == 2) {
                        sql::Statement* stmt = con->createStatement();
                        sql::ResultSet* res = stmt->executeQuery("SELECT * FROM patients");

                        cout << "\n===== Patient List =====\n";
                        while (res->next()) {
                            cout << res->getInt("id") << " | "
                                << res->getString("name") << " | "
                                << res->getInt("age") << " | "
                                << res->getString("disease") << " | RM"
                                << res->getDouble("totalBill") << endl;
                        }

                        delete res;
                        delete stmt;
                    }

                    else if (choice == 3) {
                        break;
                    }
                }
            }

            else if (role == 3) {
                cout << "Exiting system...\n";
                break;
            }

            else {
                cout << "Invalid option.\n";
            }
        }

        delete con;
    }

    catch (sql::SQLException& e) {
        cout << "Database Error: " << e.what() << endl;
    }

    return 0;
}