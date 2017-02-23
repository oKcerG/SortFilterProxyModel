import QtQuick 2.0
import QtQml 2.2
import QtTest 1.1
import SortFilterProxyModel 0.2

Item {
    ListModel {
        id: dataModel
        ListElement {
            firstName: "Tupac"
            lastName: "Shakur"
        }
        ListElement {
            firstName: "Charles"
            lastName: "Aznavour"
        }
    }
    SortFilterProxyModel {
        id: testModel
        sourceModel: dataModel
    }
    TestCase {
        name: "Helper functions"

        function test_getWithRoleName() {
            compare(testModel.get(0, "lastName"), "Shakur");
        }

        function test_getWithoutRoleName() {
            compare(testModel.get(1), { firstName: "Charles", lastName: "Aznavour"});
        }

        function test_roleForName() {
            compare(testModel.data(testModel.index(0, 0), testModel.roleForName("firstName")), "Tupac");
            compare(testModel.data(testModel.index(1, 0), testModel.roleForName("lastName")), "Aznavour");
        }
    }
}
