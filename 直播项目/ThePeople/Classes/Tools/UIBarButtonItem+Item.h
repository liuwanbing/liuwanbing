//
//  UIBarButtonItem+Item.h
//  ThePeople
//
//  Created by aoyolo on 16/4/12.
//  Copyright © 2016年 Ms.Gao. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIBarButtonItem (Item)

+ (UIBarButtonItem *)barButtonItemWithImage:(NSString *)image highImage:(NSString *)highImage target:(id)target action:(SEL)action forControlEvents:(UIControlEvents)controlEvents;

@end
