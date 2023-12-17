
#include "Misc/AutomationTest.h"
#include "MapViewer.h"

BEGIN_DEFINE_SPEC(FMapViewerTests, "MapViewerTests", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FMapViewerTests)

void FMapViewerTests::Define()
{
	Describe("Constructor", [this]()
	{
		It("should be successfully constructed", [this]()
		{
			const UMapViewerTestHelper* TestViewer = NewObject<UMapViewerTestHelper>();
			TestNotNull("MapViewerTestHelper should not be null", TestViewer);
		});
	});

	Describe("Scaling Factor Handling", [this]()
	{
		// Test Case 1: Map Display Adjustment Based on Scaling Factor (Square)
		It("should scale the map (square) to fit the widget", [this]()
		{
			UMapViewerTestHelper* TestViewer = NewObject<UMapViewerTestHelper>();

			TestViewer->FakeConstruct(FVector2D(1920, 1080), FVector2D(500, 500));

			// Use the GetScalingFactor method to calculate the actual scaling factors
			const FVector2D ScalingFactors = TestViewer->CalculateScalingFactorExposed();

			// Calculate the expected scaled map size using the scaling factors
			const FVector2D ExpectedScaledSize = FVector2D(500.0f * ScalingFactors.X, 500.0f * ScalingFactors.Y);

			// Assert that the scaled map size is as expected
			TestEqual("Scaled map width is correct", ExpectedScaledSize.X, 1080.0); // The scaled width should fit the widget width
			TestEqual("Scaled map height is correct", ExpectedScaledSize.Y, 1080.0); // The scaled height should fit the widget height
		});

		// Test Case 2: Map Display Adjustment Based on Scaling Factor (Rectangular)
		It("should scale the map (rectangle) to fit the widget", [this]()
		{
			UMapViewerTestHelper* TestViewer = NewObject<UMapViewerTestHelper>();

			// Manually initialize the viewer for the test environment
			TestViewer->FakeConstruct(FVector2D(1920, 1080), FVector2D(800, 600));

			const FVector2D ScalingFactors = TestViewer->CalculateScalingFactorExposed(); // Get scaling factors

			// Apply scaling factors to map size
			const FVector2D ScaledMapSize = FVector2D(800.0f * ScalingFactors.X, 600.0f * ScalingFactors.Y);

			// Check if scaled map width and height fits within widget
			const bool WidthFits = ScaledMapSize.X <= 1920.0f;
			const bool HeightFits = ScaledMapSize.Y <= 1080.0f;

			// Assert that the scaled map fits within the widget and maintains aspect ratio
			TestTrue("Scaled map width fits within widget", WidthFits);
			TestTrue("Scaled map height fits within widget", HeightFits);

			// Calculate the expected dimensions based on the aspect ratio
			const double ExpectedHeight = (ScaledMapSize.X * 3) / 4; // Based on 4:3 aspect ratio
			TestEqual("Scaled map height maintains aspect ratio", ScaledMapSize.Y, ExpectedHeight);
		});

		// Test Case 3: Map Larger Than Widget Size
		It("should correctly scale a larger non-square map to fit within the widget", [this]()
		{
			UMapViewerTestHelper* TestViewer = NewObject<UMapViewerTestHelper>();

			// Manually initialize the viewer for the test environment
			TestViewer->FakeConstruct(FVector2D(500, 500), FVector2D(1000, 800)); // Widget size, Non-square larger map size

			// Calculate the scaling factors
			const FVector2D ScalingFactors = TestViewer->CalculateScalingFactorExposed();

			// Apply scaling factors to map size
			const FVector2D ScaledMapSize = FVector2D(1000.0f * ScalingFactors.X, 800.0f * ScalingFactors.Y);

			// Check if scaled map width and height fits within widget
			const bool WidthFits = ScaledMapSize.X <= 500.0f;
			const bool HeightFits = ScaledMapSize.Y <= 500.0f;

			// Assert that the scaled map fits within the widget
			TestTrue("Scaled map width fits within widget", WidthFits);
			TestTrue("Scaled map height fits within widget", HeightFits);

			// Check aspect ratio maintenance
			constexpr float ExpectedAspectRatio = 1000.0f / 800.0f;
			const float ScaledAspectRatio = ScaledMapSize.X / ScaledMapSize.Y;
			TestEqual("Scaled map maintains aspect ratio", ScaledAspectRatio, ExpectedAspectRatio);
		});

	});

	Describe("Zoom Functionality", [this]()
	{

		It("should adjust the viewport size correctly when zooming in and out", [this]()
		{
			const FVector2D CursorPosition(250, 250);


			UMapViewerTestHelper* TestViewer = NewObject<UMapViewerTestHelper>();

			// Manually initialize the viewer for the test environment
			TestViewer->FakeConstruct(FVector2D(500, 500), FVector2D(500, 500));

			FVector2D InitialViewportSize = TestViewer->GetViewportSize();

			// Simulate zoom in
			FPointerEvent FakeZoomInEvent = UMapViewerTestHelper::MakeFakeScrollMouseEvent(1.0f, CursorPosition); // Positive delta for zooming in
			TestViewer->NativeOnMouseWheelExposed(FGeometry(), FakeZoomInEvent);

			// Check if viewport size decreased after zooming in
			FVector2D PostZoomInViewportSize = TestViewer->GetViewportSize();
			TestTrue("Viewport size decreased on zoom in", PostZoomInViewportSize.X < InitialViewportSize.X && PostZoomInViewportSize.Y < InitialViewportSize.Y);

			// Simulate zoom out with a more significant zoom out action
			FPointerEvent FakeZoomOutEvent = UMapViewerTestHelper::MakeFakeScrollMouseEvent(-2.0f, CursorPosition); // Negative delta for zooming out
			TestViewer->NativeOnMouseWheelExposed(FGeometry(), FakeZoomOutEvent);

			// Check if viewport size increased or returned to initial size after zooming out
			FVector2D PostZoomOutViewportSize = TestViewer->GetViewportSize();
			TestTrue("Viewport size increased or returned to initial on zoom out", PostZoomOutViewportSize.X > PostZoomInViewportSize.X && PostZoomOutViewportSize.Y > PostZoomInViewportSize.Y);
		});

		It("should adjust viewport to show only center point after zooming in", [this]()
		{
			const FVector2D CursorPosition(250, 250);

			UMapViewerTestHelper* TestViewer = NewObject<UMapViewerTestHelper>();

			// Manually initialize the viewer for the test environment
			TestViewer->FakeConstruct(FVector2D(500, 500), FVector2D(500, 500));

			TestEqual("Viewport Position is located at center of scaled map", FVector2D(250, 250), TestViewer->GetViewportPosition());

			// Simulate zoom in
			const FPointerEvent FakeZoomInEvent = UMapViewerTestHelper::MakeFakeScrollMouseEvent(100.0f, CursorPosition); // Positive delta for zooming in
			TestViewer->NativeOnMouseWheelExposed(FGeometry(), FakeZoomInEvent);

			// Check if the center point (250, 250) is visible
			const FVector2D CenterPoint = FVector2D(250, 250);
			bool IsCenterVisible = TestViewer->IsPointWithinViewportExposed(CenterPoint);

			// Check if the point at (500, 500) is no longer visible
			const FVector2D CornerPoint = FVector2D(499, 499);
			bool IsCornerVisible = TestViewer->IsPointWithinViewportExposed(CornerPoint);

			// Assert that only the center point is visible
			TestTrue("Center point is visible after zooming in", IsCenterVisible);
			TestFalse("Corner point is not visible after zooming in", IsCornerVisible);

			// Simulate zoom out
			const FPointerEvent FakeZoomOutEvent = UMapViewerTestHelper::MakeFakeScrollMouseEvent(-100.0f, CursorPosition); // Negative delta for zooming out
			TestViewer->NativeOnMouseWheelExposed(FGeometry(), FakeZoomOutEvent);

			// Update the visibility checks after zooming out
			IsCenterVisible = TestViewer->IsPointWithinViewportExposed(CenterPoint);
			IsCornerVisible = TestViewer->IsPointWithinViewportExposed(CornerPoint);

			// Assert that both points are visible after zooming out
			TestTrue("Center point is visible after zooming out", IsCenterVisible);
			TestTrue("Corner point is visible after zooming out", IsCornerVisible);
		});

		It("should maintain CurrentZoomLevel within normalized bounds of 0 to 1", [this]()
		{
			const FVector2D CursorPosition(250, 250);
			UMapViewerTestHelper* TestViewer = NewObject<UMapViewerTestHelper>();

			// Manually initialize the viewer for the test environment
			TestViewer->FakeConstruct(FVector2D(500, 500), FVector2D(500, 500));

			// Simulate extreme zoom out (e.g., delta of -100)
			FPointerEvent FakeZoomOutEvent = UMapViewerTestHelper::MakeFakeScrollMouseEvent(-100.0f, CursorPosition);
			TestViewer->NativeOnMouseWheelExposed(FGeometry(), FakeZoomOutEvent);

			// Verify CurrentZoomLevel is at minimum (0)
			float CurrentZoomLevel = TestViewer->GetCurrentZoomLevel();
			TestTrue("CurrentZoomLevel is at minimum after extreme zoom out", FMath::IsNearlyEqual(CurrentZoomLevel, 0.0f, KINDA_SMALL_NUMBER));

			// Simulate extreme zoom in (e.g., delta of 100)
			FPointerEvent FakeZoomInEvent = UMapViewerTestHelper::MakeFakeScrollMouseEvent(100.0f, CursorPosition);
			TestViewer->NativeOnMouseWheelExposed(FGeometry(), FakeZoomInEvent);

			// Verify CurrentZoomLevel is at maximum (1)
			CurrentZoomLevel = TestViewer->GetCurrentZoomLevel();
			TestTrue("CurrentZoomLevel is at maximum after extreme zoom in", FMath::IsNearlyEqual(CurrentZoomLevel, 1.0f, KINDA_SMALL_NUMBER));
		});
	});

	Describe("Panning Functionality", [this]()
	{
		// Test to verify initial viewport position for different map and widget size ratios
		It("Viewport Position On Startup Should be Center of Scaled Map", [this]()
		{
			// Scenario 1: Map Wider Than Widget
			{
				UMapViewerTestHelper* TestViewer = NewObject<UMapViewerTestHelper>();
				const FVector2D WidgetSize(600, 400); // Example widget size
				const FVector2D MapSize(1000, 800);   // Wider map size

				TestViewer->FakeConstruct(WidgetSize, MapSize);

				const FVector2D ScalingFactor = TestViewer->CalculateScalingFactorExposed();

				// Calculate the expected viewport center for a wider map
				const FVector2D ExpectedViewportCenter = FVector2D(MapSize.X * ScalingFactor.X / 2.0f, MapSize.Y * ScalingFactor.Y / 2.0f);
				const FVector2D ActualViewportCenter = TestViewer->GetViewportPosition();

				// Verify that the actual viewport center matches the expected viewport center
				TestEqual("Viewport is centered on a wider map", ActualViewportCenter, ExpectedViewportCenter);
			}

			// Scenario 2: Map Taller than Widget
			{
				UMapViewerTestHelper* TestViewer = NewObject<UMapViewerTestHelper>();
				const FVector2D WidgetSize(400, 600); // Example widget size
				const FVector2D MapSize(800, 1000);   // Taller map size

				TestViewer->FakeConstruct(WidgetSize, MapSize);

				const FVector2D ScalingFactor = TestViewer->CalculateScalingFactorExposed();

				// Calculate the expected viewport center for a taller map
				const FVector2D ExpectedViewportCenter = FVector2D(MapSize.X * ScalingFactor.X / 2.0f, MapSize.Y * ScalingFactor.Y / 2.0f);
				const FVector2D ActualViewportCenter = TestViewer->GetViewportPosition();

				// Verify that the actual viewport center matches the expected viewport center
				TestEqual("Viewport is centered on a taller map", ActualViewportCenter, ExpectedViewportCenter);
			}
		});

		// Test to verify that the viewport position updates on panning
		It("should update the viewport position on panning within bounds", [this]()
		{
			UMapViewerTestHelper* TestViewer = NewObject<UMapViewerTestHelper>();

			// Initialize the viewer with a specific widget and map size
			TestViewer->FakeConstruct(FVector2D(600, 400), FVector2D(1000, 800));

			// Simulate initial conditions: viewport is centered
			const FVector2D InitialViewportPosition = TestViewer->GetViewportPosition();

			// Calculate the scaled map size
			const FVector2D ScalingFactor = TestViewer->CalculateScalingFactorExposed();
			const FVector2D ScaledMapSize = FVector2D(1000, 800) * ScalingFactor;
			const FVector2D HalfViewportSize = TestViewer->GetViewportSize() / 2.0f;

			// Simulate a mouse button down event to start panning
			TestViewer->SetIsPanning(true);

			// Define the movement vector for panning (e.g., moving 50 units to the right and 30 units down)
			const FVector2D MovementDelta(50, 30);

			// Simulate panning
			TestViewer->RecalculateViewportPositionExposed(MovementDelta);

			// Simulate a mouse button up event to end panning
			TestViewer->SetIsPanning(false);

			// Get the updated viewport position
			const FVector2D UpdatedViewportPosition = TestViewer->GetViewportPosition();

			// Calculate the expected new viewport position with clamping
			FVector2D ExpectedViewportPosition = InitialViewportPosition + MovementDelta;
			ExpectedViewportPosition.X = FMath::Clamp(ExpectedViewportPosition.X, HalfViewportSize.X, ScaledMapSize.X - HalfViewportSize.X);
			ExpectedViewportPosition.Y = FMath::Clamp(ExpectedViewportPosition.Y, HalfViewportSize.Y, ScaledMapSize.Y - HalfViewportSize.Y);

			// Check if the viewport position has been updated correctly
			TestEqual("Viewport position updated on panning within bounds", UpdatedViewportPosition, ExpectedViewportPosition);
		});

		// Test to Verify Panning Stops when going up against Map Border
		It("should adjust viewport position to stay within scaled map bounds on panning", [this]()
		{
			UMapViewerTestHelper* TestViewer = NewObject<UMapViewerTestHelper>();

			// Initialize the viewer with specific widget and map size
			TestViewer->FakeConstruct(FVector2D(600, 400), FVector2D(1000, 800));

			// Calculate the scaled size of the map and scaling factor
			const FVector2D ScalingFactor = TestViewer->CalculateScalingFactorExposed();
			const FVector2D ScaledMapSize = FVector2D(1000, 800) * ScalingFactor;

			// Set an initial viewport position near the edge of the scaled map
			const FVector2D HalfViewportSize = TestViewer->GetViewportSize() / 2.0f;
			const FVector2D EdgePosition = ScaledMapSize - HalfViewportSize; // Adjust to be near the edge
			TestViewer->SetViewportPosition(EdgePosition);

			// Simulate a mouse button down event to start panning
			TestViewer->SetIsPanning(true);

			// Define a movement vector that might move the viewport outside the scaled map bounds
			const FVector2D MovementDelta(100, 50); // Adjust as needed

			// Simulate panning
			TestViewer->RecalculateViewportPositionExposed(MovementDelta);

			// Simulate a mouse button up event to end panning
			TestViewer->SetIsPanning(false);

			// Get the updated viewport position
			const FVector2D UpdatedViewportPosition = TestViewer->GetViewportPosition();

			// Manually compute the expected clamped position for each component
			FVector2D ExpectedClampedPosition;
			ExpectedClampedPosition.X = FMath::Clamp(EdgePosition.X + MovementDelta.X, HalfViewportSize.X, ScaledMapSize.X - HalfViewportSize.X);
			ExpectedClampedPosition.Y = FMath::Clamp(EdgePosition.Y + MovementDelta.Y, HalfViewportSize.Y, ScaledMapSize.Y - HalfViewportSize.Y);

			// Check if the viewport position is clamped correctly
			TestEqual("Viewport position is clamped within scaled map bounds", UpdatedViewportPosition, ExpectedClampedPosition);
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

	Describe("Misc Tests", [this]()
	{
		// Near Edge of Map, and zooming out keeps us within bounds
		It("Near Edge of Map, and zooming out keeps us within bounds", [this]()
		{
			UMapViewerTestHelper* TestViewer = NewObject<UMapViewerTestHelper>();

			// Initialize the viewer with a specific widget and map size
			TestViewer->FakeConstruct(FVector2D(600, 400), FVector2D(1000, 800));

			// Set the viewport position near the edge of the map and zoom in
			TestViewer->SetViewportPosition(FVector2D(800, 600)); // Near edge position
			TestViewer->SetCurrentZoomLevel(0.5f); // Example zoom level

			// Simulate zoom out
			const FPointerEvent FakeZoomOutEvent = UMapViewerTestHelper::MakeFakeScrollMouseEvent(-1.0f, FVector2D(800, 600)); // Negative delta for zooming out
			TestViewer->NativeOnMouseWheelExposed(FGeometry(), FakeZoomOutEvent);

			// Get the updated viewport position
			const FVector2D UpdatedViewportPosition = TestViewer->GetViewportPosition();

			// Calculate the scaled map size
			const FVector2D ScalingFactor = TestViewer->CalculateScalingFactorExposed();
			const FVector2D ScaledMapSize = FVector2D(1000, 800) * ScalingFactor;

			// Manually compute the expected clamped position for each component
			FVector2D ClampedViewportPosition;
			const FVector2D HalfViewportSize = TestViewer->GetViewportSize() / 2.0f;
			ClampedViewportPosition.X = FMath::Clamp(UpdatedViewportPosition.X, HalfViewportSize.X, ScaledMapSize.X - HalfViewportSize.X);
			ClampedViewportPosition.Y = FMath::Clamp(UpdatedViewportPosition.Y, HalfViewportSize.Y, ScaledMapSize.Y - HalfViewportSize.Y);

			// Check if the viewport position is still within bounds after zooming out
			TestEqual("Viewport position is within bounds after zooming out", UpdatedViewportPosition, ClampedViewportPosition);
		});

		// Test to verify that the viewport adjusts correctly to the cursor position on zoom
		It("Viewport Adjusts to Cursor Position on Zoom", [this]()
		{
			UMapViewerTestHelper* TestViewer = NewObject<UMapViewerTestHelper>();

			// Initialize the viewer with a specific widget and map size
			TestViewer->FakeConstruct(FVector2D(600, 400), FVector2D(1000, 800));

			// Define the cursor position for zooming in
			const FVector2D CursorPosition(300, 200);

			// Simulate moving the cursor to the position
			TestViewer->SetIsPanning(true);
			FPointerEvent FakeMoveEvent = TestViewer->MakeFakeMoveMouseEvent(CursorPosition, CursorPosition, FVector2D(0.0f, 0.0f));
			TestViewer->NativeOnMouseMoveExposed(FGeometry(), FakeMoveEvent);
			TestViewer->SetIsPanning(false);

			// Simulate zoom in
			FPointerEvent FakeZoomInEvent = TestViewer->MakeFakeScrollMouseEvent(1.0f, CursorPosition);
			TestViewer->NativeOnMouseWheelExposed(FGeometry(), FakeZoomInEvent);

			// Get the updated viewport size
			const FVector2D NewViewportSize = TestViewer->GetViewportSize();
			const FVector2D ScalingFactor = TestViewer->CalculateScalingFactorExposed();
			const FVector2D ScaledMapSize = FVector2D(1000, 800) * ScalingFactor;

			// Calculate the expected viewport position after zooming in (accounting for clamping)
			const FVector2D ExpectedViewportPositionAfterZoom = FVector2D(
				FMath::Clamp(CursorPosition.X - (NewViewportSize.X / 2.0f), NewViewportSize.X / 2.0f, ScaledMapSize.X - NewViewportSize.X / 2.0f),
				FMath::Clamp(CursorPosition.Y - (NewViewportSize.Y / 2.0f), NewViewportSize.Y / 2.0f, ScaledMapSize.Y - NewViewportSize.Y / 2.0f)
			);

			// Get the updated viewport position
			const FVector2D UpdatedViewportPosition = TestViewer->GetViewportPosition();

			// Check if the viewport position is correctly adjusted to the cursor position after zooming in
			TestEqual("Viewport correctly adjusts to cursor position on zoom", UpdatedViewportPosition, ExpectedViewportPositionAfterZoom);
		});
	});

}
