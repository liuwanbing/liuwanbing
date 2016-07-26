//
//  GGTransverseNaV.h
//  ThePeople
//
//  Created by aoyolo on 16/4/14.
//  Copyright © 2016年 Ms.Gao. All rights reserved.
//
//横向的nav
#import <UIKit/UIKit.h>

@protocol GGTransverseNaVDelegate<NSObject>
-(void)TransverseNaVWillDidOnclick:(NSInteger)tag;
@end

@interface GGTransverseNaV : UIView
@property (nonatomic , weak)id<GGTransverseNaVDelegate>delegate;
@end
