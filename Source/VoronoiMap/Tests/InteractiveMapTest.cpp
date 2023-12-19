
#include "Misc/AutomationTest.h"
#include "InteractiveMap.h"

BEGIN_DEFINE_SPEC(FInteractiveMapTests, "InteractiveMapTests", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FInteractiveMapTests)

void FInteractiveMapTests::Define()
{
	Describe("General Setup", [this]()
	{
		It("should instantiate the UInteractiveMap class as an object", [this]()
		{
			// Arrange & Act
			UMapViewerTestHelper* MapViewer = NewObject<UMapViewerTestHelper>();
			MapViewer->SetData(FVector2D(800, 600), FVector2D(1000, 1000));

			// Assert
			TestNotNull(TEXT("MapViewer should not be null"), MapViewer);
		});

		It("should correctly get and set the Widget Size", [this]()
		{
			// Arrange
			const FVector2D ExpectedSize = FVector2D(800, 600);
			UMapViewerTestHelper* MapViewer = NewObject<UMapViewerTestHelper>();

			// Act
			MapViewer->SetData(ExpectedSize, FVector2D(1000, 1000));
			const FVector2D ActualSize = MapViewer->GetWidgetSize();


			// Assert
			TestEqual(TEXT("Widget size should be set and retrieved correctly"), ActualSize, ExpectedSize);
		});

		It("should correctly get and set the Map Size", [this]()
		{
			// Arrange
			const FVector2D ExpectedSize = FVector2D(1000, 1000);
			UMapViewerTestHelper* MapViewer = NewObject<UMapViewerTestHelper>();

			// Act
			MapViewer->SetData(FVector2D(800, 600), ExpectedSize);
			const FVector2D ActualSize = MapViewer->GetMapSize();

			// Assert
			TestEqual(TEXT("Map size should be set and retrieved correctly"), ActualSize, ExpectedSize);
		});

		It("should correctly position the map position at the center of the map on startup", [this]()
		{
			// Arrange
			const FVector2D WidgetSize = FVector2D(800, 600);
			const FVector2D MapSize = FVector2D(1000, 1000);
			UMapViewerTestHelper* MapViewer = NewObject<UMapViewerTestHelper>();

			// Act
			MapViewer->SetData(WidgetSize, MapSize);
			const FVector2D ActualMapPosition = MapViewer->GetViewportPosition();

			// Assert
			TestEqual(TEXT("Viewport should be positioned at the center of the map"), ActualMapPosition, MapSize / 2);
		});
	});

	Describe("Panning Functionality", [this]()
	{
		It("should pan the map in the same direction as the mouse movement", [this]()
		{
			// Arrange
			UMapViewerTestHelper* MapViewer = NewObject<UMapViewerTestHelper>();
			MapViewer->SetData(FVector2D(800, 600), FVector2D(5000, 5000)); // WidgetSize and MapSize
			MapViewer->SetPanningLimitsEnabled(false); // Assuming panning limits can be disabled for this test

			FVector2D InitialViewportPosition = MapViewer->GetViewportPosition();

			// Simulate mouse movement
			FVector2D MouseDelta = FVector2D(30, 30); // Moving mouse right and down
			FGeometry MyGeometry; // Initialize appropriately
			FPointerEvent MouseEvent = UMapViewerTestHelper::MakeFakeMoveMouseEvent(FVector2D(100, 100), FVector2D(130, 130), MouseDelta);

			// Act
			MapViewer->NativeOnMouseMoveExposed(MyGeometry, MouseEvent);
			FVector2D NewViewportPosition = MapViewer->GetViewportPosition();

			// Assert
			// Check if the viewport moved in the opposite direction of the mouse (left and up, due to subtraction)
			TestTrue(TEXT("Viewport should move left with mouse movement"), NewViewportPosition.X - InitialViewportPosition.X < 0);
			TestTrue(TEXT("Viewport should move up with mouse movement"), NewViewportPosition.Y - InitialViewportPosition.Y < 0);
		});


		It("Should Stop Panning on Border", [this]()
		{
			// Initialize the UInteractiveMap instance
			UMapViewerTestHelper* MapViewer = NewObject<UMapViewerTestHelper>();
			MapViewer->SetData(FVector2D(500, 500), FVector2D(1000, 1000));
			MapViewer->SetPanningLimitsEnabled(true);

			// Geometry used for the mouse events
			FGeometry DummyGeometry;

			// Simulate panning motion to the right
			FVector2D StartCursorPos(250, 250);
			FVector2D EndCursorPos(350, 250);
			FVector2D CursorDelta = EndCursorPos - StartCursorPos;

			FPointerEvent MouseEvent = UMapViewerTestHelper::MakeFakeMoveMouseEvent(EndCursorPos, StartCursorPos, CursorDelta);
			MapViewer->NativeOnMouseMoveExposed(DummyGeometry, MouseEvent);

			// Manually check if viewport position is within expected boundaries
			bool bIsWithinXBounds = MapViewer->GetViewportPosition().X >= 250 && MapViewer->GetViewportPosition().X <= 500;
			bool bIsWithinYBounds = MapViewer->GetViewportPosition().Y >= 250 && MapViewer->GetViewportPosition().Y <= 500;

			TestTrue("Viewport X position should be within bounds", bIsWithinXBounds);
			TestTrue("Viewport Y position should be within bounds", bIsWithinYBounds);
		});

	});

	Describe("Zoom Functionality", [this]()
	{
		It("Should ensure that the zoom level is normalized", [this]()
		{
			// Arrange
			UMapViewerTestHelper* MapViewer = NewObject<UMapViewerTestHelper>();
			MapViewer->SetData(FVector2D(800, 600), FVector2D(5000, 5000));
			FGeometry MyGeometry; // Initialize appropriately

			// Act - Simulate extreme scroll in (zooming in)
			FPointerEvent ScrollInEvent = UMapViewerTestHelper::MakeFakeScrollMouseEvent(10.0f, FVector2D(100, 100));
			MapViewer->NativeOnMouseWheelExposed(MyGeometry, ScrollInEvent);
			float ZoomLevelAfterScrollIn = MapViewer->GetZoomLevel();

			// Act - Simulate extreme scroll out (zooming out)
			FPointerEvent ScrollOutEvent = UMapViewerTestHelper::MakeFakeScrollMouseEvent(-10.0f, FVector2D(100, 100));
			MapViewer->NativeOnMouseWheelExposed(MyGeometry, ScrollOutEvent);
			float ZoomLevelAfterScrollOut = MapViewer->GetZoomLevel();

			// Assert
			TestTrue(TEXT("Zoom level should be clamped at or below 1 after scrolling in"), ZoomLevelAfterScrollIn <= 1.0f);
			TestTrue(TEXT("Zoom level should be clamped at or above 0 after scrolling out"), ZoomLevelAfterScrollOut >= 0.0f);
		});

		It("Should correctly calculate ViewportSize based on zoom level", [this]()
		{
			// Arrange
			UMapViewerTestHelper* MapViewer = NewObject<UMapViewerTestHelper>();
			MapViewer->SetData(FVector2D(800, 600), FVector2D(5000, 5000)); // WidgetSize and MapSize
			const FVector2D MinViewportSize = FVector2D(50, 50); // Predetermined size at max zoom
			const FVector2D CursorPosition = FVector2D(100, 100); // Example cursor position

			// Act & Assert for different scroll amounts
			for (float ScrollAmount = -10.0f; ScrollAmount <= 10.0f; ScrollAmount += 1.0f)
			{
				FGeometry MyGeometry;
				FPointerEvent ScrollEvent = UMapViewerTestHelper::MakeFakeScrollMouseEvent(ScrollAmount, CursorPosition);
				MapViewer->NativeOnMouseWheelExposed(MyGeometry, ScrollEvent);

				// Expected viewport size based on current zoom level
				FVector2D ExpectedViewportSize = FMath::Lerp(MapViewer->GetMapSize(), MinViewportSize, MapViewer->GetZoomLevel());

				TestEqual<FVector2D>(TEXT("ViewportSize should be correctly calculated"), MapViewer->GetViewportSize(), ExpectedViewportSize);
			}
		});

		It("Should Reposition if needed on Zooming", [this]()
		{
			// Arrange
			UMapViewerTestHelper* MapViewer = NewObject<UMapViewerTestHelper>();
			MapViewer->SetData(FVector2D(500, 500), FVector2D(1000, 1000)); // Example sizes

			// Set an initial ViewportPosition that would require adjustment after zooming out
			MapViewer->SetViewportPosition(FVector2D(800, 800)); // Example position

			// Simulate a zoom out action
			float WheelDelta = -1.0f; // Negative for zooming out
			FVector2D CursorPosition = FVector2D(250, 250); // Example cursor position
			FGeometry DummyGeometry; // This needs to be set up or mocked appropriately

			FPointerEvent MouseEvent = UMapViewerTestHelper::MakeFakeScrollMouseEvent(WheelDelta, CursorPosition);
			MapViewer->NativeOnMouseWheelExposed(DummyGeometry, MouseEvent);

			// Act
			// Calculate top-left corner based on the current center (ViewportPosition)
			FVector2D ViewportTopLeft = MapViewer->GetViewportPosition() - (MapViewer->GetViewportPosition() / 2);

			// Adjust the top-left corner to keep it within map boundaries
			ViewportTopLeft.X = FMath::Max(0.0f, ViewportTopLeft.X);
			ViewportTopLeft.Y = FMath::Max(0.0f, ViewportTopLeft.Y);

			// Adjust for bottom-right corner
			FVector2D BottomRightCorner = ViewportTopLeft + MapViewer->GetViewportSize();
			if (BottomRightCorner.X > MapViewer->GetMapSize().X)
			{
				ViewportTopLeft.X -= (BottomRightCorner.X - MapViewer->GetMapSize().X);
			}
			if (BottomRightCorner.Y > MapViewer->GetMapSize().Y)
			{
				ViewportTopLeft.Y -= (BottomRightCorner.Y - MapViewer->GetMapSize().Y);
			}

			// Recalculate the expected center position
			FVector2D ExpectedViewportPosition = ViewportTopLeft + (MapViewer->GetViewportSize() / 2);

			// Assert
			TestEqual("Viewport should be repositioned to stay within the map", MapViewer->GetViewportPosition(), ExpectedViewportPosition);
		});
	});
}
