#include <iostream>
#include <vector>
#include <string>
#include "ElevatorController.h"
#include "ElevatorDoor.h"
#include "ElevatorEngine.h"
#include "ElevatorButton.h"
#include "elevator_utils.h"
#include "ElevatorsManager.h"

int main() {
    ElevatorEngine engine(0.0f, 5); // 6 floors (0 to 5)
    ElevatorDoor door;
    std::vector<InsideFloorButton> insideButtons(6);
    for (int i = 0; i < 6; i++) 
    {
        insideButtons[i].floor = i;
    }

    ElevatorButton stopButton;
    ElevatorButton closeButton;
    ElevatorButton openButton;
    ElevatorController controller(
        6, door, engine, 
        insideButtons.begin(), insideButtons.end(), 
        &stopButton, &closeButton, &openButton
    );
    std::vector<OutsideFloorButton> outsideButtons;
    outsideButtons.reserve(6*2);
    for (int i = 0; i < 6; i++)
    {
        outsideButtons.push_back(OutsideFloorButton(i, Direction::UP));
        outsideButtons.push_back(OutsideFloorButton(i, Direction::DOWN));
    }
    ElevatorsManager mgr(&controller, &(controller)+1, outsideButtons.begin(), outsideButtons.end());
    // TODO: add buttons to menu
    
    // Subscribe to controller updates for testing
    controller.subscribeOnElevatorStateChanged([&](const ElevatorControllerUpdate& update) {
        std::cout << "Elevator Update: " << std::endl;
        std::cout << "  State: " << stateToStr(update.state) << "\n" << update.controller->getEngine() << update.controller->getDoor() << "\n";
        if (!update.cancelled_requests.empty()) {
            std::cout << "  Cancelled Requests: ";
            for (int floor : update.cancelled_requests) {
                std::cout << floor << " ";
            }
            std::cout << std::endl;
        }
        });

    // Simple CLI interaction loop
    while (true) {
        std::cout << "\nElevator CLI Test" << std::endl;
        std::cout << "1. Press Inside Floor Button" << std::endl;
        std::cout << "2. Press Stop Button" << std::endl;
        std::cout << "3. Press Close Button" << std::endl;
        std::cout << "4. Press Open Button" << std::endl;
        std::cout << "5. Update (Simulate Time)" << std::endl;
        std::cout << "6. Print Engine and Door status" << std::endl;
        std::cout << "7. Press Outside Floor Button (UP)" << std::endl;
        std::cout << "8. Press Outside Floor Button (DOWN)" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            int floor;
            std::cout << "Enter floor number (0-5): ";
            std::cin >> floor;
            if (floor >= 0 && floor <= 5) {
                insideButtons[floor].press();
            }
            else {
                std::cout << "Invalid floor number." << std::endl;
            }
        }
        else if (choice == 2) {
            stopButton.press();
        }
        else if (choice == 3) {
            closeButton.press();
        }
        else if (choice == 4) {
            openButton.press();
        }
        else if (choice == 5) {
            float deltaTime;
            std::cout << "Enter delta time (seconds): ";
            std::cin >> deltaTime;
            engine.update(deltaTime);
            door.update(deltaTime);
        }
        else if (choice == 6) {
            std::cout << engine;
            std::cout << door;
        }
        else if (choice == 7) {
            int floor;
            std::cout << "Enter floor number (0-5): ";
            std::cin >> floor;
            if (floor >= 0 && floor <= 5) {
                outsideButtons[floor * 2].press(); // UP button
            }
            else {
                std::cout << "Invalid floor number." << std::endl;
            }
        }
        else if (choice == 8) {
            int floor;
            std::cout << "Enter floor number (0-5): ";
            std::cin >> floor;
            if (floor >= 0 && floor <= 5) {
                outsideButtons[floor * 2 + 1].press(); // DOWN button
            }
            else {
                std::cout << "Invalid floor number." << std::endl;
            }
        }
        else if (choice == 0) {
            break;
        }
        else {
            std::cout << "Invalid choice." << std::endl;
        }
    }

    return 0;
}