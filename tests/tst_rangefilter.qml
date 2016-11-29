import QtQuick 2.0
import SortFilterProxyModel 0.2
import QtQml.Models 2.2
import QtTest 1.1

Item {
    id: topLevelItem

    ListModel {
        id: dataModel0
        ListElement { value: 5 }
        ListElement { value: 3 }
        ListElement { value: 1 }
        ListElement { value: 2 }
        ListElement { value: 4 }
    }
    ListModel {
        id: dataModel1
        ListElement { value: 5 }
        ListElement { value: 2 }
        ListElement { value: 3 }
        ListElement { value: 1 }
        ListElement { value: 4 }
    }

    SortFilterProxyModel { id: testModel0; sourceModel: dataModel0;
        // Test that rangeFilter defaults to inclusive min and max
        property string tag: "model 0"  // If this is not defined, QtTest barfs using these models as the _data array.
        filters: RangeFilter { roleName: "value"; minimumValue: 2; maximumValue: 4 }
        property int expectedModelCount: 3
        property var expectedValues: [3, 2, 4]
    }
    SortFilterProxyModel { id: testModel1; sourceModel: dataModel0;
        // Test explicit inclusive min and max
        property string tag: "model 1"
        filters: RangeFilter { roleName: "value"; minimumValue: 2; maximumValue: 4; minimumInclusive: true; maximumInclusive: true }
        property int expectedModelCount: 3
        property var expectedValues: [3, 2, 4]
    }
    SortFilterProxyModel { id: testModel2; sourceModel: dataModel1
        // Test inclusive min, exclusive max
        property string tag: "model 2"
        filters: RangeFilter { roleName: "value"; minimumValue: 2; maximumValue: 4; minimumInclusive: true; maximumInclusive: false }
        property int expectedModelCount: 2
        property var expectedValues: [2, 3]
    }
    SortFilterProxyModel { id: testModel3; sourceModel: dataModel1
        // Test exclusive min, inclusive max
        property string tag: "model 3"
        filters: RangeFilter { roleName: "value"; minimumValue: 2; maximumValue: 4; minimumInclusive: false; maximumInclusive: true }
        property int expectedModelCount: 2
        property var expectedValues: [3, 4]
    }
    SortFilterProxyModel { id: testModel4; sourceModel: dataModel1
        // Test exclusive min and max
        property string tag: "model 4"
        filters: RangeFilter { roleName: "value"; minimumValue: 2; maximumValue: 4; minimumInclusive: false; maximumInclusive: false }
        property int expectedModelCount: 1
        property var expectedValues: [3]
    }
    SortFilterProxyModel { id: testModel5; sourceModel: dataModel1
        // Test inverted range (expect no hits)
        property string tag: "model 5"
        filters: RangeFilter { roleName: "value"; minimumValue: 4; maximumValue: 2 }
        property int expectedModelCount: 0
    }


    TestCase {
        name:"RangeFilterTests"

        function test_minMax_data() {
            return Array.prototype.slice.call(topLevelItem.resources, 2);
        }

        function test_minMax(model) {
            verify(model.count === model.expectedModelCount,
                   "Expected count " + model.expectedModelCount + ", actual count: " + model.count);
            for (var i = 0; i < model.count; i++)
            {
                var modelValue = model.data(model.index(i, 0), "value");
                verify(modelValue === model.expectedValues[i],
                       "Expected testModel value " + modelValue + ", actual: " + model.expectedValues[i]);
            }
        }
    }
}
