import QtQuick 2.0
import SortFilterProxyModel 0.2
import QtQml.Models 2.2
import QtTest 1.1

Item {
    id: topLevelItem
    SortFilterProxyModel {
        id: testModel

        filters : RangeFilter {
            id: rangeFilter
        }

    }

    TestCase {
        name:"RangeFilterTests"

        function initModel(data) {
            if (testModel.sourceModel === null) {
                testModel.sourceModel = Qt.createQmlObject("import QtQml.Models 2.2; ListModel {}", topLevelItem, "listmodel");
            } else {
                testModel.sourceModel.clear();
            }
            for (var i = 0; data.modelData !== undefined && i < data.modelData.length; i++)
            {
                testModel.sourceModel.set(i, { value: data.modelData[i] });
            }

            rangeFilter.minimumValue = data.rangeMinValue;
            rangeFilter.maximumValue = data.rangeMaxValue;
            if (data.rangeMinInclusive !== undefined)
                rangeFilter.minimumInclusive = data.rangeMinInclusive;
            if (data.rangeMaxInclusive !== undefined)
                rangeFilter.maximumInclusive = data.rangeMaxInclusive;
            rangeFilter.roleName = "value";
            data.testRole = rangeFilter.roleName;
        }

        function test_minMax_data() {
            return [{
                // Test that rangeFilter defaults to inclusive min and max
                modelData: [5,3,1,2,4],
                rangeMinValue: 2,
                rangeMaxValue: 4,
                expectedModelCount: 3,
                expectedValues: [3, 2, 4]
            },
            {
                // Test explicit inclusive min and max
                modelData: [5,3,1,2,4],
                rangeMinValue: 2,
                rangeMaxValue: 4,
                rangeMinInclusive: true,
                rangeMaxInclusive: true,
                expectedModelCount: 3,
                expectedValues: [3, 2, 4]
            },
            {
                // Test inclusive min, exclusive max
                modelData: [5,2,3,1,4],
                rangeMinValue: 2,
                rangeMaxValue: 4,
                rangeMinInclusive: true,
                rangeMaxInclusive: false,
                expectedModelCount: 2,
                expectedValues: [2, 3]
            },
            {
                // Test exclusive min, inclusive max
                modelData: [5,2,3,1,4],
                rangeMinValue: 2,
                rangeMaxValue: 4,
                rangeMinInclusive: false,
                rangeMaxInclusive: true,
                expectedModelCount: 2,
                expectedValues: [3, 4]
            },
            {
                // Test exclusive min and max
                modelData: [5,2,3,1,4],
                rangeMinValue: 2,
                rangeMaxValue: 4,
                rangeMinInclusive: false,
                rangeMaxInclusive: false,
                expectedModelCount: 1,
                expectedValues: [3]
            },
            {
                // Test inverted range (expect no hits)
                modelData: [5,2,3,1,4],
                rangeMinValue: 4,
                rangeMaxValue: 2,
                rangeMinInclusive: true,
                rangeMaxInclusive: true,
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
