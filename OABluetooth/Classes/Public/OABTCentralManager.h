//
//  OABLECentralManager.h
//  OutdoorAssistantApplication
//
//  Created by 罗亮富 on 2018/11/10.
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import "NSObject+MultiDelegates.h"

typedef enum {
    OABTCentralStateUnknow = 0,
    OABTCentralStateResetting,
    OABTCentralStateUnsupported,
    OABTCentralStateUnauthorized,
    OABTCentralStatePoweredOff,
    OABTCentralStatePoweredOn,
    OABLECentralStateScanning
    
}OABTCentralState;

#define ENABLE_OABT_LOG 0

NS_ASSUME_NONNULL_BEGIN

@class OABTCentralManager;

//Use delgate or blocks according to your preferences
//根据你个人喜好，可以选择使用block或delegate方式
@protocol OABlePeripheralManagerDelegate <NSObject>

//this delegate method will be invoked whenever power on/off or authrization status change
-(void)centralManagerDidChangeState:(OABTCentralManager *)manager;

//this delegate method will be invoked on new peripherals were discovered
-(void)centralManager:(OABTCentralManager *)manager didDiscoveredNewPeripherals:(NSArray <CBPeripheral *>*)peripherals;

//连接断开、建立连接、连接完成
//invoked whenever connection state changed.
-(void)centralManager:(OABTCentralManager *)manager didChangeStateForPeripheral:(CBPeripheral *)peripheral;


@end

/*!
 *  @class OABLECentralManager
 *
 *  @discussion Manage peripherals' scan, connection.
 */
@interface OABTCentralManager : NSObject<CBCentralManagerDelegate,CBPeripheralDelegate>


-(instancetype)init NS_UNAVAILABLE;


/**
 designated initializer
 @param advertiseID     the uuid string representing the service advertising by peripherals to scan for, nil for all kinds of peripherals
    需要扫描的外设广播id,只有带此广播id的外设才会被扫描到，如果传入nil的话，表示扫描所有类型的外设
 */
-(instancetype)initWitPeripheralAdvertiseID:(nullable NSString *)advertiseID;

@property (nonatomic, readonly)  OABTCentralState state;


#pragma mark- auto connection 自动重连

/**
 defualt YES,whether to automatically reconnect to detected peripherals 是否在断开后自动重连
 see -addPeripheralToAutoReconnection: and -removeperipheralFromAutoReconnection:
 */
@property (nonatomic, getter = isAutoReconnectionEnabled) BOOL enableAutoReconnection;
@property (nonatomic) unsigned int autoReconnectionInterval; //default is 5 second


/**
 auto connection list adding/removing
 When enableAutoReconnection set to YES, peripherals that were added to autoconnection through -addPeripheralToAutoReconnection: will auto reconnected on detection.
 You can disable auto connection for specific peripheral throung -removeperipheralFromAutoReconnection: All set enableAutoReconnection to NO for all peripherals.
 
 当 enableAutoReconnection 设置为YES的时候, 通过 -addPeripheralToAutoReconnection:方法添加的外设，在被扫描到后将会自动连接, 你也可以通过-removeperipheralFromAutoReconnection：
 方法移除对指定设备的自动重连；或者将enableAutoReconnection设置为NO来取消所有设备的自动重连
 */
-(void)addPeripheralToAutoReconnection:(nonnull CBPeripheral *)peripheral;
-(void)removeperipheralFromAutoReconnection:(nonnull CBPeripheral *)peripheral;

-(nullable NSSet <NSString *> *)autoConnectionPeripheralUUids;

#pragma mark- auto scan 自动扫描

//自动搜索时间间隔,the time in second to start a auto scan since last scan finished, defautl is 5 seconds
@property (nonatomic) unsigned int autoScanInterval;
//每一次搜索持续时间，the scan last duration each time, default is 3 seconds
@property (nonatomic) unsigned int scanDuration;

#pragma mark- delegates/blocks

//用delegates的好处是可以多处同时监听，但代码分散
//implemented in NSObject category,
// 添加delegate
-(void)addDelegate:(id<OABlePeripheralManagerDelegate>)delegate;
//移除deletate
-(void)removeDelegate:(id<OABlePeripheralManagerDelegate>)delegate;

//block的好处是代码集中，但同时只能有一处监听，如果要多处同时监听这些事件的话，用-addDelegate:方法添加多个delegate
//Alternatively you can implement OABlePeripheralManagerDelegate's methods by -addDelegate:
@property (nonatomic, copy) void (^onBluetoothStateChange)(OABTCentralState state);
@property (nonatomic, copy) void (^onNewPeripheralsDiscovered)(NSArray <CBPeripheral *> *peripherals);
@property (nonatomic, copy) void (^onPeripheralStateChange)(CBPeripheral *peripheral);

#pragma mark- scan and connection

/**
 Start to scan for peripherals according to 'advertiseID' provided in the initializer -initWitPeripheralAdvertiseID:
 -centralManager:didDiscoverPeripheral: delegate method or ^onNewPeripheralsDiscovered() block will be invoked on every new peripheral discovered

 @return nil if the scan process can be started, else return an error to indicate the failure.
 */
-(NSError *)scanPeripherals;

/**
 stop peripheral scan
 -centralManager:didChangeStateForPeripheral: delegate methods or ^onPeripheralStateChange() block will be invoked
 */
-(void)stopScanPeripherals;

@property (nonatomic, copy, readonly) NSArray <CBPeripheral *>* discoveredPeripherals;
@property (nonatomic, copy, readonly) NSArray <CBPeripheral *> *connectedPeripherals;

/**
 Connect peripheral
 
 -centralManager:didChangeStateForPeripheral: delegate method or ^onPeripheralStateChange() block  will be invoked on every peripheral's connection
 */
-(void)connectPeripheral:(nonnull CBPeripheral *)peripheral
              completion:(nullable void (^)(NSError *error))block;


/**
 Disconnect peripheral

 断开连接 的接口不设计block回调，是因为断开连接的状态有被动和主动两种方式，程序员可以选择在设置^onPeripheralStateChange中处理，或在delegate方法中处理
 -centralManager:didChangeStateForPeripheral: delegate method or ^onPeripheralStateChange() block  will be invoked on every peripheral disconnected
 */
-(void)disConnectperipheral:(nonnull CBPeripheral *)peripheral;


@end



NS_ASSUME_NONNULL_END

