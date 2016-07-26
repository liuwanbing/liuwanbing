//
//  GGVerticalNav.h
//  ThePeople
//
//  Created by aoyolo on 16/4/14.
//  Copyright © 2016年 Ms.Gao. All rights reserved.
//
//竖屏的nav
#import <UIKit/UIKit.h>
@protocol GGVerticalNavDelegate<NSObject>
- (void)didWillVerticalNavOnclick:(NSInteger)tag;
@end

@interface GGVerticalNav : UIView

@property (nonatomic , weak)id <GGVerticalNavDelegate>delegate;

@end
