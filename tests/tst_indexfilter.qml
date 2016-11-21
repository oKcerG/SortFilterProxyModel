import QtQuick 2.0
import SortFilterProxyModel 0.2
import QtQml.Models 2.2
import QtTest 1.1

Item {
    id: topLevelItem
    SortFilterProxyModel {
        id: testModel
    }

    TestCase {
        name:"IndexFilterTests"

        function initModel(data) {
            data.filter = Qt.createQmlObject("import SortFilterProxyModel 0.2; IndexFilter { id: filter }", testModel, "filter");
            testModel.filters = data.filter;

            if (testModel.sourceModel === null) {
                testModel.sourceModel = Qt.createQmlObject("import QtQml.Models 2.2; ListModel {}", topLevelItem, "listmodel");
            } else {
                testModel.sourceModel.clear();
            }
            for (var i = 0; data.modelData !== undefined && i < data.modelData.length; i++)
            {
                testModel.sourceModel.set(i, { value: data.modelData[i] });
            }

            data.filter.minimumIndex = data.min;
            data.filter.maximumIndex = data.max;
            if (data.inverted)
                data.filter.inverted = true;

            data.testRole = "value";
        }

        function test_minMax_data() {
            return [
            {
                // Test basic IndexFilter usage
                modelData: [5,3,1,2,4],
                min: 1,
                max: 3,
                expectedModelCount: 3,
                expectedValues: [3, 1, 2]
            },
            {
                // Test inverted range (expect no hits)
                modelData: [5,2,3,1,4],
                min: 3,
                max: 1,
                expectedModelCount: 0,
            },
            {
                modelData: [5,3,1,2,4],
                min: 0,
                max: 0,
                inverted: true,
                expectedModelCount: 4,
                expectedValues: [3,1,2,4]
            },
            {
                // Test issue #15 - min max of 0 containing the value at index 0.
                modelData: [5,3,1,2,4],
                min: 0,
                max: 0,
                expectedModelCount: 1,
                expectedValues: [5]
            },
            {
                modelData: [5,2,3,1,4],
                min: 1,
                max: 3,
                inverted: true,
                expectedModelCount: 2,
                expectedValues: [5,4]
            },
            ];
        }

        function test_minMax(data) {
            initModel(data);

            verify(testModel.count === data.expectedModelCount,
                   "Expected count " + data.expectedModelCount + ", actual count: " + testModel.count);
            for (var i = 0; i < testModel.count; i++)
            {
                var modelValue = testModel.data(testModel.index(i, 0), data.testRole);
                verify(modelValue === data.expectedValues[i],
                       "Expected testModel value " + modelValue + ", actual: " + data.expectedValues[i]);
            }
        }
    }
}
