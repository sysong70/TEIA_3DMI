#pragma once

#include "../3DF/3DF.h"
#include "Sprocket.h"

#include "../3DF/KeyPath.h"

namespace H3DF
{
    using ComponentArray = std::vector<Component *, Allocator<Component *>>;

    class API_3DF Component : public Sprocket
	{
	public:
        enum class Type : uint32_t {
            None = 0x00000000,
            GenericMask = 0xfffff000,

            ExchangeComponentMask = 0x00001000, // Mask for any Exchange components.
            ExchangeModelFile =
                0x00001001, // Represents an A3DAsmModelFile in Exchange.  This will be represented by an Exchange::CADModel.
            ExchangeProductOccurrence = 0x00001002, // Represents an A3DAsmProductOccurrence in Exchange.  This will be
                                                    // represented by an Exchange::Component.
            ExchangePartDefinition = 0x00001003, // Represents an A3DAsmPartDefinition in Exchange.  This will be represented by
                                                 // an Exchange::Component.

            ExchangeMkpView = 0x00001004, // Represents an A3DMkpView in Exchange.  This will be represented by an
                                       // Exchange::Capture component.
            ExchangeFilter = 0x00001005, // Represents an A3DMkpFilter in Exchange.  This will be represented by an
                                         // Exchange::Filter component.

            ExchangeRepresentationItemMask = 0x00003000, // Mask for any Exchange representation items.
            ExchangeRIBRepModelSolid = 0x00003001,
            ExchangeRIBRepModelSurface = 0x00003002,
            ExchangeRICurve = 0x00003003,
            ExchangeRIDirection = 0x00003004,
            ExchangeRIPlane = 0x00003005,
            ExchangeRIPointSet = 0x00003006,
            ExchangeRIPolyBRepModelSolid = 0x00003007,
            ExchangeRIPolyBRepModelSurface = 0x00003008,
            ExchangeRIPolyWire = 0x00003009,
            ExchangeRISet = 0x0000300a,
            ExchangeRICoordinateSystem = 0x0000300b,

            ExchangeTopologyMask = 0x00005000, // Mask for any Exchange topology items.
            ExchangeTopoBody =
                0x00005001, // Represents an A3DTopoBody in Exchange.  This will be represented by an Exchange::Component.
            ExchangeTopoConnex =
                0x00005002, // Represents an A3DTopoConnex in Exchange.  This will be represented by an Exchange::Component.
            ExchangeTopoShell =
                0x00005003, // Represents an A3DTopoShell in Exchange.  This will be represented by an Exchange::Component.
            ExchangeTopoFace =
                0x00005004, // Represents an A3DTopoFace in Exchange.  This will be represented by an Exchange::Component.
            ExchangeTopoLoop =
                0x00005005, // Represents an A3DTopoLoop in Exchange.  This will be represented by an Exchange::Component.
            ExchangeTopoCoEdge =
                0x00005006, // Represents an A3DTopoCoEdge in Exchange.  This will be represented by an Exchange::Component.
            ExchangeTopoEdge =
                0x00005007, // Represents an A3DTopoEdge in Exchange.  This will be represented by an Exchange::Component.
            ExchangeTopoVertex =
                0x00005008, // Represents an A3DTopoVertex in Exchange.  This will be represented by an Exchange::Component.
            ExchangeTopoSingleWireBody = 0x00005009, // Represents an A3DTopoSingleWireBody in Exchange.  This will be
                                                     // represented by an Exchange::Component.
            ExchangeTopoWireEdge =
                0x0000500a, // Represents an A3DTopoWireEdge in Exchange.  This will be represented by an Exchange::Component.

            ExchangeDrawingMask = 0x00009000, // Mask for any Exchange drawing item.
            ExchangeDrawingModel =
                0x00009001, // Represents an A3DDrawingModel in Exchange. This is the starting point of a drawing. It is found
                            // under a product occurrence. Generally there is only one Drawing Model per file, but it is
                            // possible to have more than one. This will be represented by an Exchange::Component.
            ExchangeDrawingView =
                0x00009002, // Represents an A3DDrawingView in Exchange. A Drawing Sheet can contain views. Views are basically
                            // snapshot of the model from a particular angle. Each view has a name and a type, for example,
                            // front view, side view, section, etc. This will be represented by an Exchange::Component.
            ExchangeDrawingSheet =
                0x00009003, // Represents an A3DDrawingSheet in Exchange. A DrawingModel contains one or more sheets. Each sheet
                            // represents a physical sheet of paper. Generally a sheet defines the frame of the drawing,
                            // contains the name of the paper format it reproduces, and contains a transform. One of the sheets
                            // is classified as the "active sheet" by the model, and this one will be what you see when you
                            // first load a drawing. There can only be one active sheet at the time, with all the inactive ones
                            // being hidden. Drawing sheets have metadata associated with them called 'IsDefaultSheet'. This is
                            // a BooleanMetadata. It will return true if the sheet is the active sheet, and false otherwise.
                            // This will be represented by an Exchange::Component.
            ExchangeBasicDrawingBlock =
                0x00009004, // Represents an A3DDrawingBlockBasic in Exchange. Sheets and views contain blocks. Blocks can be of
                            // two types: basic or operator blocks. Operator blocks are blocks which reference a particular
                            // view. These two types are internally treated as the same thing. Blocks contain the actual
                            // representation items which will be tessellated and drawn on the screen. This will be represented
                            // by an Exchange::Component.
            ExchangeOperatorDrawingBlock =
                0x00009005, // Represents an A3DDrawingBlockOperator in Exchange. Sheets and views contain blocks. Blocks can be
                            // of two types: basic or operator blocks. Operator blocks are blocks which reference a particular
                            // view. These two types are internally treated as the same thing. Blocks contain the actual
                            // representation items which will be tessellated and drawn on the screen. This will be represented
                            // by an Exchange::Component.

            ExchangePMIMask = 0x00011000, // Mask for any Exchange PMI items.
            ExchangePMI = ExchangePMIMask, // Represents a generic A3DMkpMarkup in Exchange.  This will be represented by an
                                           // Exchange::Component.
            ExchangePMIText =
                0x00011100, // Represents an A3DMarkupText in Exchange.  This will be represented by an Exchange::Component.
            ExchangePMIRichText =
                0x00011200, // Represents an A3DMarkupRichText in Exchange.  This will be represented by an Exchange::Component.
            ExchangePMIRoughness = 0x00011300, // Represents an A3DMarkupRoughness in Exchange.  This will be represented by an
                                               // Exchange::Component.
            ExchangePMIGDT =
                0x00011400, // Represents an A3DMarkupGDT in Exchange.  This will be represented by an Exchange::Component.
            ExchangePMIDatum =
                0x00011500, // Represents an A3DMarkupDatum in Exchange.  This will be represented by an Exchange::Component.
            ExchangePMILineWelding = 0x00011600, // Represents an A3DMarkupLineWelding in Exchange.  This will be represented by
                                                 // an Exchange::Component.
            ExchangePMISpotWelding = 0x00011700, // Represents an A3DMarkupSpotWelding in Exchange.  This will be represented by
                                                 // an Exchange::Component.
            ExchangePMIDimension = 0x00011800, // Represents an A3DMarkupDimension in Exchange.  This will be represented by an
                                               // Exchange::Component.
            ExchangePMIBalloon =
                0x00011900, // Represents an A3DMarkupBalloon in Exchange.  This will be represented by an Exchange::Component.
            ExchangePMICoordinate = 0x00011a00, // Represents an A3DMarkupCoordinate in Exchange.  This will be represented by
                                                // an Exchange::Component.
            ExchangePMIFastener =
                0x00011b00, // Represents an A3DMarkupFastener in Exchange.  This will be represented by an Exchange::Component.
            ExchangePMILocator =
                0x00011c00, // Represents an A3DMarkupLocator in Exchange.  This will be represented by an Exchange::Component.
            ExchangePMIMeasurementPoint = 0x00011d00, // Represents an A3DMarkupMeasurementPoint in Exchange.  This will be
                                                      // represented by an Exchange::Component.
            DWGComponentMask = 0x00100000, // Mask for any DWG components.
            DWGModelFile = 0x00100001, // Represents an AcDbDatabase in RealDWG.  This will be represented by a DWG::CADModel.
            DWGLayout = 0x00100002, // Represents an AcDbLayout in RealDWG.
            DWGBlockTable = 0x00100003, // Represents an AcDbBlockTable in RealDWG.
            DWGBlockTableRecord = 0x00100004, // Represents an AcDbBlockTableRecord in RealDWG.
            DWGEntity = 0x00100005, // Represents an AcDbEntity in RealDWG.
            DWGLayerTable = 0x00100006, // Represents an AcDbLayerTable in RealDWG.
            DWGLayer = 0x00100007, // Represents an AcDbLayer in RealDWG.

            UserComponent = 0x01000000, // Represents a user created component

            ModelsComponent = 0x01000001, // Represents a model component
            MeasurementsComponent = 0x01000002, // Represents a model component
            MarkupsComponent = 0x01000003, // Represents a model component
            ViewGroupComponent = 0x01000004, // UI에서 View를 Group으로 나타내기 위한 Type;
            PMIGroupComponent = 0x01000005, // UI에서 Pmi를 Group으로 나타내기 위한 Type;

        };

		enum Status
		{
            None = 0x0000,
			End = 0x0001,
			UiUpdate = 0x0002,
			Hide = 0x0004,			// 원래 Hide된 경우, Reset할때 사용하기 위한 Status
			NoShow = 0x0008,		// NoShow된 경우
		};

		Component();
		Component(Component const & cInThat);

		void Set(Component const & cInThat);
		Component & operator = (Component const & cInThat);

        H3DF::Type ObjectType() const { return H3DF::Type::Component; };

        bool Equals(Component const & cInThat) const;

        H3DF::Component::Type GetType() const;

        HC_KEY GetSegmentKey() const;
        HC_KEY GetIncludeKey() const;

        Component & GetOwner() const;

        ComponentArray & GetSubComponents() const;
        size_t GetAllSubComponentCount() const;

        CString GetName() const;

        DWORD GetStatus();
		DWORD AddStatus(Component::Status eStatus);
		DWORD RemoveStatus(Component::Status eStatus);

        bool IsRepresentationItem();

        static KeyPath GetKeyPath(Component const & cInComponent);
	};
}
