#include "Misc/AutomationTest.h"
#include "MapViewer.h"

BEGIN_DEFINE_SPEC(FMapViewerTests, "MapViewerTests", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)


END_DEFINE_SPEC(FMapViewerTests)


class MapViewerTestHelper : public UMapViewer
{
private:


public:
	MapViewerTestHelper(const FObjectInitializer& ObjectInitializer) : UMapViewer(ObjectInitializer) {}

	static FPointerEvent MakeFakeScrollMouseEvent(const float WheelDelta)
	{
		return FPointerEvent(0, FVector2D(0.0f, 0.0f), FVector2D(0.0f, 0.0f), TSet<FKey>(), FKey(), WheelDelta, FModifierKeysState());
	}

	FVector2D TestViewportSize;

	void SetTestViewportSize(const FVector2D& NewSize)
	{
		TestViewportSize = NewSize;
	}

	virtual float GetAspectRatio() const override
	{
		return TestViewportSize.X / TestViewportSize.Y;
	}

	FVector2D GetViewportSize() const
	{
		return UMapViewer::GetCachedGeometry().GetLocalSize();
	}

	FReply CallNativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		return NativeOnMouseWheel(InGeometry, InMouseEvent);
	}

	static FPointerEvent MakeFakeMoveMouseEvent(const FVector2D& CursorPosition, const FVector2D& LastCursorPosition, const FVector2D& CursorDelta)
	{
		return FPointerEvent(0, CursorPosition, LastCursorPosition, CursorDelta, TSet<FKey>(), FModifierKeysState());
	}

	FReply CallNativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		return NativeOnMouseMove(InGeometry, InMouseEvent);
	}
};


void FMapViewerTests::Define()
{

	Describe("Constructor", [this]()
	{
		It("should be successfully constructed", [this]()
		{
			const MapViewerTestHelper* TestViewer = NewObject<MapViewerTestHelper>();
			TestNotNull("MapViewerTestHelper should not be null", TestViewer);
		});
	});

	Describe("Aspect Ratio Handling", [this]()
	{
		// Test Case 0: Getter/Setters
		It("should correctly set and get the viewport size", [this]()
		{
			MapViewerTestHelper* TestViewer = NewObject<MapViewerTestHelper>();
			FVector2D ExpectedSize = FVector2D(1200.0f, 800.0f);
			TestViewer->SetTestViewportSize(ExpectedSize);
			FVector2D ActualSize = TestViewer->TestViewportSize;
			TestEqual("Viewport size should be set and retrieved correctly", ActualSize, ExpectedSize);
		});


		// Test Case 1: Correct Aspect Ratio Calculation
		It("should correctly calculate the aspect ratio", [this]()
		{
			MapViewerTestHelper* TestViewer = NewObject<MapViewerTestHelper>();
			TestViewer->SetTestViewportSize(FVector2D(1200.0f, 800.0f)); // Set a known viewport size

			constexpr float ExpectedAspectRatio = 1200.0f / 800.0f; // Calculate the expected aspect ratio
			const float CalculatedAspectRatio = TestViewer->GetAspectRatio(); // Get the aspect ratio from the viewer

			TestEqual("Aspect ratio should be calculated correctly", CalculatedAspectRatio, ExpectedAspectRatio);
		});

		// Test Case 2: Map Display Adjustment Based on Aspect Ratio
		It("should adjust the map display based on aspect ratio", [this]()
		{
			MapViewerTestHelper* TestViewer = NewObject<MapViewerTestHelper>();

			TestViewer->SetTestViewportSize(FVector2D(1200.0f, 800.0f)); // Set initial viewport size
			const FVector2D InitialMapSize = TestViewer->GetMapSize(); // Store the initial map size for comparison

			TestViewer->SetTestViewportSize(FVector2D(1600.0f, 900.0f)); // Change the viewport size
			const FVector2D AdjustedMapSize = TestViewer->GetMapSize(); // Get the adjusted map size

			// Assuming that the map should adjust its size while maintaining the aspect ratio
			const bool IsAspectRatioMaintained = FMath::Abs(AdjustedMapSize.X / AdjustedMapSize.Y - InitialMapSize.X / InitialMapSize.Y) < KINDA_SMALL_NUMBER;

			TestTrue("Map should adjust its display while maintaining aspect ratio", IsAspectRatioMaintained);
		});
	});


	Describe("Dynamic Viewport Size Handling", [this]()
	{
		It("should adapt to increased viewport size", [this]()
		{
			// Setup for increasing the viewport size
			// Test if the map scales correctly
		});

		It("should adapt to decreased viewport size", [this]()
		{
			// Setup for decreasing the viewport size
			// Test if the map scales correctly
		});
	});


	Describe("Zoom Functionality", [this]()
	{
		It("should adjust zoom level correctly", [this]()
		{
			MapViewerTestHelper* TestViewer = NewObject<MapViewerTestHelper>();

			const FPointerEvent FakeMouseEvent = MapViewerTestHelper::MakeFakeScrollMouseEvent(1.0f);

			TestViewer->CallNativeOnMouseWheel(FGeometry(), FakeMouseEvent);

			TestTrue("Zoom level should be within 0.1 and 1.0", TestViewer->CurrentZoomLevel >= 0.1f && TestViewer->CurrentZoomLevel <= 1.0f);
		});
	});

	Describe("Pan Functionality", [this]()
	{
		It("should update the map position correctly on panning", [this]()
		{
			MapViewerTestHelper* TestViewer = NewObject<MapViewerTestHelper>();
			TestViewer->IsPanning = true;

			// Initial position for reference
			const FVector2D InitialMapPos = TestViewer->MapPosition;

			const FPointerEvent FakeMouseEvent = MapViewerTestHelper::MakeFakeMoveMouseEvent(FVector2D(100.0f, 100.0f), FVector2D(50.0f, 50.0f), FVector2D(50.0f, 50.0f));

			TestViewer->CallNativeOnMouseMove(FGeometry(), FakeMouseEvent);

			const FVector2D NewMapPos = TestViewer->MapPosition;

			// Verify that the map position has been updated
			TestNotEqual("MapPosition should have been updated", InitialMapPos, NewMapPos);

			TestViewer->IsPanning = false;
		});

		It("should respect map boundaries with different viewport sizes", [this]()
		{
			// Setup for a specific viewport size
			// Test panning to ensure the map boundaries are respected
		});
	});

	Describe("Zoom Level Adjustment", [this]()
	{
		It("should maintain correct zoom levels with different aspect ratios", [this]()
		{
			// Setup for varying aspect ratios
			// Test if the zoom levels are maintained correctly
		});
	});


	Describe("Clipping Logic", [this]()
	{
		It("should correctly clip points outside the viewport", [this]()
		{
			// Setup for a specific viewport and map size
			// Test if points outside the viewport are correctly clipped
		});

		It("should display points within the viewport", [this]()
		{
			// Setup for a specific viewport and map size
			// Test if points within the viewport are correctly displayed
		});
	});


	Describe("Extreme Cases Handling", [this]()
	{
		It("should handle very small viewport sizes", [this]()
		{
			// Setup for a very small viewport size
			// Test if the map still functions correctly
		});

		It("should handle very large viewport sizes", [this]()
		{
			// Setup for a very large viewport size
			// Test if the map still functions correctly
		});
	});
}
