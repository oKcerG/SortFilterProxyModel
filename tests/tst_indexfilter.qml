import QtQuick 2.0
import SortFilterProxyModel 0.2
import QtQml.Models 2.2
import QtTest 1.1

Item {
    id: topLevelItem

    ListModel {
        id: dataModel
        ListElement { value: 5 }
        ListElement { value: 3 }
        ListElement { value: 1 }
        ListElement { value: 2 }
        ListElement { value: 4 }
    }

    SortFilterProxyModel { id: testModel0; sourceModel: dataModel;
        property string tag: "model 0"  // If this is not defined, QtTest barfs using these models as the _data array.
        filters: IndexFilter { minimumIndex: 1; maximumIndex: 3 }
        // Test basic IndexFilter usage
        property int expectedModelCount: 3
        property var expectedValues: [3, 1, 2];
    }
    SortFilterProxyModel { id: testModel1; sourceModel: dataModel;
        property string tag: "model 1"
        filters: IndexFilter { minimumIndex: 3; maximumIndex: 1 }
        // Test inverted range (expect no hits)
        property int expectedModelCount: 0
    }
    SortFilterProxyModel { id: testModel2; sourceModel: dataModel;
        property string tag: "model 2"
        filters: IndexFilter { minimumIndex: 0; maximumIndex: 0; inverted: true }
        property int expectedModelCount: 4
        property var expectedValues: [3,1,2,4];
    }
    SortFilterProxyModel { id: testModel3; sourceModel: dataModel;
        // Test issue #15 - min max of 0 containing the value at index 0.
        property string tag: "model 3"
        filters: IndexFilter { minimumIndex: 0; maximumIndex: 0 }
        property int expectedModelCount: 1
        property var expectedValues: [5];
    }
    SortFilterProxyModel { id: testModel4; sourceModel: dataModel;
        property string tag: "model 4"
        filters: IndexFilter { minimumIndex: 1; maximumIndex: 3; inverted: true }
        property int expectedModelCount: 2
        property var expectedValues: [5,4]
    }


    TestCase {
        name:"IndexFilterTests"

        function test_minMax_data() {
            return Array.prototype.slice.call(topLevelItem.resources, 1);
        }

        function test_minMax(model) {
            verify(model.count === model.expectedModelCount,
                   "Expected count " + model.expectedModelCount + ", actual count: " + model.count);
            for (var i = 0; i < model.count; i++)
            {
                var modelValue = model.data(model.index(i, 0));
                verify(modelValue === model.expectedValues[i],
                       "Expected testModel value " + modelValue + ", actual: " + model.expectedValues[i]);
            }
        }
    }
}
