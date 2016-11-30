import QtQuick 2.0
import SortFilterProxyModel 0.2
import QtQml.Models 2.2
import QtTest 1.1

Item {
    property list<IndexFilter> filters: [
        IndexFilter {
            minimumIndex: 1; maximumIndex: 3
            property string tag: "basicUsage"
            property int expectedModelCount: 3
            property var expectedValues: [3, 1, 2];
        },
        IndexFilter {
            minimumIndex: 3; maximumIndex: 1
            property string tag: "outOfBounds"
            property int expectedModelCount: 0
        },
        IndexFilter {
            minimumIndex: 0; maximumIndex: 0; inverted: true
            property string tag: "0to0Inverted"
            property int expectedModelCount: 4
            property var expectedValues: [3,1,2,4]
        },
        IndexFilter {
            minimumIndex: 0; maximumIndex: 0
            property string tag: "0to0" // bug / issue #15
            property int expectedModelCount: 1
            property var expectedValues: [5]
        },
        IndexFilter {
            minimumIndex: 1; maximumIndex: 3; inverted: true
            property string tag: "basicUsageInverted"
            property int expectedModelCount: 2
            property var expectedValues: [5,4]
       }
    ]

    SortFilterProxyModel {
        id: testModel
        property ListModel myModel: ListModel {
            ListElement { value: 5 }
            ListElement { value: 3 }
            ListElement { value: 1 }
            ListElement { value: 2 }
            ListElement { value: 4 }
        }
        sourceModel: myModel // Crashes/fails with error if I define ListModel directly here.
    }

    TestCase {
        name: "IndexFilterTests"

        function test_minMax_data() {
            return filters;
        }

        function test_minMax(filter) {
            testModel.filters = filter

            verify(testModel.count === filter.expectedModelCount,
                   "Expected count " + filter.expectedModelCount + ", actual count: " + testModel.count);
            for (var i = 0; i < testModel.count; i++)
            {
                var modelValue = testModel.data(testModel.index(i, 0));
                verify(modelValue === filter.expectedValues[i],
                       "Expected testModel value " + modelValue + ", actual: " + filter.expectedValues[i]);
            }
        }
    }
}
