import QtQuick 2.0
import SortFilterProxyModel 0.2
import QtQml.Models 2.2
import QtTest 1.1

Item {
    property list<ListModel> dataModels: [
    ListModel {
        ListElement { value: 5 }
        ListElement { value: 3 }
        ListElement { value: 1 }
        ListElement { value: 2 }
        ListElement { value: 4 }
    },
    ListModel {
        ListElement { value: 5 }
        ListElement { value: 2 }
        ListElement { value: 3 }
        ListElement { value: 1 }
        ListElement { value: 4 }
    }
    ]

    property list<RangeFilter> filters: [
        RangeFilter {
            roleName: "value"; minimumValue: 2; maximumValue: 4
            property string tag: "inclusive"
            property int expectedModelCount: 3
            property var expectedValues: [3, 2, 4]
            property QtObject dataModel: dataModels[0]
        },
        RangeFilter {
            roleName: "value"; minimumValue: 2; maximumValue: 4; minimumInclusive: true; maximumInclusive: true
            property string tag: "explicitInclusive"
            property int expectedModelCount: 3
            property var expectedValues: [3, 2, 4]
            property QtObject dataModel: dataModels[0]
        },
        RangeFilter {
            roleName: "value"; minimumValue: 2; maximumValue: 4; minimumInclusive: true; maximumInclusive: false
            property string tag: "inclusiveMinExclusiveMax"
            property int expectedModelCount: 2
            property var expectedValues: [2, 3]
            property QtObject dataModel: dataModels[1]
        },
        RangeFilter {
            roleName: "value"; minimumValue: 2; maximumValue: 4; minimumInclusive: false; maximumInclusive: true
            property string tag: "exclusiveMinInclusiveMax"
            property int expectedModelCount: 2
            property var expectedValues: [3, 4]
            property QtObject dataModel: dataModels[1]
        },
        RangeFilter {
            roleName: "value"; minimumValue: 2; maximumValue: 4; minimumInclusive: false; maximumInclusive: false
            property string tag: "exclusive"
            property int expectedModelCount: 1
            property var expectedValues: [3]
            property QtObject dataModel: dataModels[1]
        },
        RangeFilter { roleName: "value"; minimumValue: 4; maximumValue: 2
            property string tag: "outOfBoundsRange"
            property int expectedModelCount: 0
            property QtObject dataModel: dataModels[1]
        }
    ]

    SortFilterProxyModel { id: testModel }

    TestCase {
        name:"RangeFilterTests"

        function test_minMax_data() {
            return filters;
        }

        function test_minMax(filter) {
            testModel.sourceModel = filter.dataModel
            testModel.filters = filter

            verify(testModel.count === filter.expectedModelCount,
                   "Expected count " + filter.expectedModelCount + ", actual count: " + testModel.count);
            for (var i = 0; i < testModel.count; i++)
            {
                var modelValue = testModel.data(testModel.index(i, 0), "value");
                verify(modelValue === filter.expectedValues[i],
                       "Expected testModel value " + modelValue + ", actual: " + filter.expectedValues[i]);
            }
        }
    }
}
