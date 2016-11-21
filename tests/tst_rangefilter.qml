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
        name:"RangeFilterTests"

        function initModel(data) {
            data.filter = Qt.createQmlObject("import SortFilterProxyModel 0.2; RangeFilter { id: filter }", testModel, "filter");
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

            data.filter.minimumValue = data.minValue;
            data.filter.maximumValue = data.maxValue;
            if (data.minInclusive !== undefined)
                data.filter.minimumInclusive = data.minInclusive;
            if (data.maxInclusive !== undefined)
                data.filter.maximumInclusive = data.maxInclusive;
            data.filter.roleName = "value";
            data.testRole = data.filter.roleName;
        }

        function test_minMax_data() {
            return [{
                // Test that rangeFilter defaults to inclusive min and max
                modelData: [5,3,1,2,4],
                minValue: 2,
                maxValue: 4,
                expectedModelCount: 3,
                expectedValues: [3, 2, 4]
            },
            {
                // Test explicit inclusive min and max
                modelData: [5,3,1,2,4],
                minValue: 2,
                maxValue: 4,
                minInclusive: true,
                maxInclusive: true,
                expectedModelCount: 3,
                expectedValues: [3, 2, 4]
            },
            {
                // Test inclusive min, exclusive max
                modelData: [5,2,3,1,4],
                minValue: 2,
                maxValue: 4,
                minInclusive: true,
                maxInclusive: false,
                expectedModelCount: 2,
                expectedValues: [2, 3]
            },
            {
                // Test exclusive min, inclusive max
                modelData: [5,2,3,1,4],
                minValue: 2,
                maxValue: 4,
                minInclusive: false,
                maxInclusive: true,
                expectedModelCount: 2,
                expectedValues: [3, 4]
            },
            {
                // Test exclusive min and max
                modelData: [5,2,3,1,4],
                minValue: 2,
                maxValue: 4,
                minInclusive: false,
                maxInclusive: false,
                expectedModelCount: 1,
                expectedValues: [3]
            },
            {
                // Test inverted range (expect no hits)
                modelData: [5,2,3,1,4],
                minValue: 4,
                maxValue: 2,
                minInclusive: true,
                maxInclusive: true,
                expectedModelCount: 0,
            }];
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
